#include <stdio.h>
#include <stdlib.h>  // exit()
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <zlib.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr
#include <time.h> 
#include "GZReader.h"
#include "Util.h"
#include <direct.h>
using namespace std;

void print_usage(const char* progname) {
	fprintf(stderr, "Extract tracks from vital file into another vital file.\n\n\
Usage : %s DNAME/TNAME INPUT1 [INPUT2] [INPUT3]\n\n\
INPUT_PATH: vital file path\n\
DEVNAME/TRKNAME : comma seperated device and track name list. ex) BIS/BIS,BIS/SEF\n\
if omitted, all tracks are copyed.\n\n", basename(progname).c_str());
}

#pragma pack(push, 1)
struct tar_header {
	char name[100];
	char mode[8];
	char owner[8];
	char group[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char type;
	char linkname[100];
	char _padding[255];
};
#pragma pack(pop)

struct tar_file {
	FILE* m_fd = nullptr;
	unsigned round_up(unsigned n, unsigned incr) {
		return n + (incr - n % incr) % incr;
	}

	unsigned long checksum(const tar_header* rh) {
		unsigned long i;
		unsigned char* p = (unsigned char*)rh;
		unsigned long res = 256;
		for (i = 0; i < offsetof(tar_header, checksum); i++) {
			res += p[i];
		}
		for (i = offsetof(tar_header, type); i < sizeof(*rh); i++) {
			res += p[i];
		}
		return res;
	}

	tar_file() {
		setmode(fileno(stdout), O_BINARY);
		m_fd = stdout;
	}

	tar_file(const char* filename) {
		m_fd = fopen(filename, "wb");
	}

	virtual ~tar_file() {
		// finalize
		write_null_bytes(sizeof(tar_header) * 2);
		if (m_fd) fclose(m_fd);
	}

private:
	bool write_null_bytes(long n) {
		char c = 0;
		for (long i = 0; i < n; i++) {
			if (!fwrite(&c, 1, 1, m_fd)) {
				return false;
			}
		}
		return true;
	}

public:
	bool write(const char* name, const vector<unsigned char>& data) {
		// Write header
		tar_header rh = { 0, };
		strcpy(rh.name, name); // ���ϸ��� �� �տ� ��
		strcpy(rh.mode, "0664");
		sprintf(rh.size, "%o", data.size());
		rh.type = '0'; // regular file

		// Calculate and write checksum
		auto chksum = checksum(&rh);
		sprintf(rh.checksum, "%06o", chksum);
		rh.checksum[7] = ' ';

		// header
		if (!fwrite(&rh, 1, sizeof(rh), m_fd)) return false;

		// Write data
		for (size_t pos = 0; pos < data.size(); pos += 512) {
			long nwrite = 512;

			// ������ write ũ�� ����
			if (pos + 512 > data.size()) nwrite = data.size() - pos;

			if (!fwrite(&data[pos], 1, nwrite, m_fd)) {
				return false;
			}

			if (nwrite != 512) { // ��� ������ �ȸ����� ä��
				return write_null_bytes(512 - nwrite);
			}
		}

		return true;
	}
};

int main(int argc, char* argv[]) {
	if (argc <= 2) { // �ּ� 1�� ���ϰ� Ʈ������ �־�� �Ѵ�.
		print_usage(argv[0]);
		return -1;
	}
	// �� ���α׷� �̸��� ����
	argc--; argv++;

	////////////////////////////////////////////////////////////
	// parse dname/tname
	////////////////////////////////////////////////////////////
	string dtname = argv[0];
	argc--; argv++;
	vector<string> tnames; // ����� Ʈ����
	vector<string> dnames; // ����� ����
	set<unsigned short> tids; // ����� tid

	tnames = explode(dtname, ',');
	long ncols = tnames.size();
	dnames.resize(ncols);
	for (long j = 0; j < ncols; j++) {
		long pos = tnames[j].find('/');
		if (pos != -1) {// devname, tname���� �и�
			dnames[j] = tnames[j].substr(0, pos);
			tnames[j] = tnames[j].substr(pos + 1);
		}
	}

	tar_file tar; // ǥ�� ��� tar ����
	for (unsigned long ifile = 0; ifile < argc; ifile++) {
		string ipath = argv[ifile];
		
		bool is_vital = true;
		if (ipath.size() < 6) is_vital = false;
		else if (to_lower(ipath.substr(ipath.size() - 6)) != ".vital") is_vital = false;
		if (!is_vital) { // vital ������ �ƴϸ� �׳� ����
			vector<unsigned char> buf;
			auto f = fopen(ipath.c_str(), "rb");
			fseek(f, 0, SEEK_END);
			auto sz = ftell(f);
			buf.resize(sz);
			rewind(f);
			fread(&buf[0], 1, sz, f); // ��ü�� �ѹ��� ����
			fclose(f);
			tar.write(basename(ipath).c_str(), buf);
			continue;
		}
		
		GZBuffer fw; // �ϳ��� vital ������ ���� ���� ����
		GZReader fr(ipath.c_str()); // ���� ������ ����.
		if (!fr.opened() || !fw.opened()) {
			fprintf(stderr, "file open error\n");
			return -1;
		}

		// header
		BUF header(10);
		char sign[4];
		if (!fr.read(sign, 4)) return -1;
		if (strncmp(sign, "VITA", 4) != 0) {
			fprintf(stderr, "file does not seem to be a vital file\n");
			return -1;
		}
		memcpy(&header[0], sign, 4);

		char ver[4];
		if (!fr.read(ver, 4)) return -1; // version
		memcpy(&header[4], ver, 4);

		unsigned short headerlen; // header length
		if (!fr.read(&headerlen, 2)) return -1;
		memcpy(&header[8], &headerlen, 2);

		header.resize(10 + headerlen);
		if (!fr.read(&header[10], headerlen)) return -1;

		fw.write(&header[0], header.size());

		map<unsigned long, string> did_dname;
		map<unsigned long, BUF> did_di;
		map<unsigned short, string> tid_tname;
		map<unsigned short, BUF> tid_ti;
		map<unsigned short, unsigned long> tid_did;
		map<unsigned short, BUF> tid_recs;

		// ������ �� ���� �����鼭 ����.
		while (!fr.eof()) { // body�� ��Ŷ�� �����̴�.
			unsigned char packet_type; if (!fr.read(&packet_type, 1)) break;
			unsigned long packet_len; if (!fr.read(&packet_len, 4)) break;
			if (packet_len > 1000000) break; // 1MB �̻��� ��Ŷ�� ����

			BUF packet_header(5);
			packet_header[0] = packet_type;
			memcpy(&packet_header[1], &packet_len, 4);

			// �ϰ��� �����ؾ��ϹǷ� �ϰ��� ���� �� �ۿ� ����
			BUF buf(packet_len);
			if (!fr.read(&buf[0], packet_len)) break;
			if (packet_type == 9) { // devinfo
				unsigned long did = 0; if (!buf.fetch(did)) continue;
				string dtype; if (!buf.fetch_with_len(dtype)) continue;
				string dname; if (!buf.fetch_with_len(dname)) continue;
				if (dname.empty()) dname = dtype;
				did_dname[did] = dname;
			}
			else if (packet_type == 0) { // trkinfo
				unsigned short tid; if (!buf.fetch(tid)) continue;
				buf.skip(2);
				string tname; if (!buf.fetch_with_len(tname)) continue;
				string tunit; buf.fetch_with_len(tunit);
				buf.skip(33);
				unsigned long did = 0; buf.fetch(did);

				tid_did[tid] = did;
				tid_tname[tid] = tname;
				auto dname = did_dname[did];

				bool matched = false;
				for (long i = 0; i < tnames.size(); i++) {
					if (tnames[i] == "*" || tnames[i] == tname) {
						if (dnames[i].empty() || dnames[i] == dname || dnames[i] == "*") {// Ʈ���� ��Ī ��
							tids.insert(tid);
							matched = true;
							break;
						}
					}
				}
				if (!matched) continue;
			} else if (packet_type == 1) { // rec
				buf.skip(10);
				unsigned short tid; if (!buf.fetch(tid)) continue;
				if (tids.find(tid) == tids.end()) continue; // ������ ���ڵ�
			}

			// �� �� ��Ŷ
			fw.write(&packet_header[0], packet_header.size());
			fw.write(&buf[0], buf.size());
		}

		tar.write(basename(ipath).c_str(), fw.m_comp);
	}

	return 0;
}