
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
using namespace std;

#include <conio.h>


class StringBuilder //
{
private:
	char* buffer_first;
	char* buffer;
	int len;
	int capacity;
private:
	char* end() const
	{
		return buffer_first + capacity;
	}
public:
	StringBuilder(const int buffer_size, const char* cstr = "", int len = 0) {
		buffer = (char*)malloc(sizeof(char) * (buffer_size + 1)); // 1 for '\0'
		this->len = len;
		capacity = buffer_size;
		memcpy(buffer, cstr, this->len);
		buffer[this->len] = '\0';
		buffer_first = buffer;
	}
	StringBuilder(const StringBuilder&) = delete;
	StringBuilder& operator=(const StringBuilder&) = delete;
	virtual ~StringBuilder()
	{
		free(buffer_first);
	}

	StringBuilder& Append(const char* cstr, const int len)
	{
		if (buffer + this->len + len < end())
		{
			memcpy(buffer + this->len, cstr, len);
			buffer[this->len + len] = '\0';
			this->len = this->len + len;
		}
		else {
			if (buffer_first + this->len + len < end()) {
				memmove(buffer_first, buffer, this->len);
				memcpy(buffer_first + this->len, cstr, len);
				buffer_first[this->len + len] = '\0';
				buffer = buffer_first;
				this->len = this->len + len;
			}
			else {
				char* new_buffer = (char*)malloc(sizeof(char) * (this->len + len + 1));
				memcpy(new_buffer, buffer, this->len);
				memcpy(new_buffer + this->len, cstr, len);
				new_buffer[this->len + len] = '\0';
				free(buffer);
				buffer = new_buffer;
				buffer_first = buffer;
				this->len = this->len + len;
			}
		}
		return *this;
	}
	const char* Divide(const int idx) // need to rename!l, chk idx range!
	{
		buffer[idx] = '\0';
		return buffer;
	}
	const char* Str(int* size = nullptr) {
		if (size) { *size = len; }
		return buffer;
	}

	void Clear()
	{
		len = 0;
		buffer = buffer_first;
		buffer[0] = '\0';
	}
	int size() { return len; }
	StringBuilder& LeftShift(const int offset = 1)
	{
		if (offset < 1) { return *this; }

		if (buffer + offset < end()) {
			buffer = buffer + offset;
		}
		else {
			memmove(buffer_first, buffer + offset, len - offset);
			buffer = buffer_first;
			buffer[len - offset] = '\0';
		}
		len = len - offset;
		return *this;
	}
};


class StringTokenizer
{
private:
	vector<string> _m_str;
	int count;
	bool exist;
	static const vector<string> whitespaces;
	int option;
private:
	void Init(string str, const vector<string>& separator, StringBuilder& builder) // assumtion : separators are sorted by length?, long -> short
	{
		if (separator.empty() || str.empty()) { return; } // if str.empty() == false then _m_str.push_back(str); // ?
														  //
		builder.Clear();
		builder.Append(str.c_str(), str.size());
		int left = 0;
		int right = 0;
		int state = 0;
		this->exist = false;

		for (int i = 0; i < str.size(); ++i) {
			right = i;
			int _select = -1;
			bool pass = false;

			if (1 == option && 0 == state && '\"' == str[i]) {
				if (i == 0) {
					state = 1;
					continue;
				}
				else if (i > 0 && '\\' == str[i - 1]) {
					throw "ERROR for Init on StringTokenizer"; // error!
				}
				else if (i > 0) {
					state = 1;
					continue;
				}
			}
			else if (1 == option && 1 == state && '\"' == str[i]) {
				if ('\\' == str[i - 1]) {
					continue;
				}
				else {
					state = 0;
				}
			}
			else if (1 == option && 1 == state) {
				continue;
			}


			for (int j = 0; j < separator.size(); ++j) {
				for (int k = 0; k < separator[j].size() && i + k < str.size(); ++k) {
					if (str[i + k] == separator[j][k]) {
						pass = true;
					}
					else {
						pass = false;
						break;
					}
				}
				if (pass) { _select = j; break; }
			}

			if (pass) {
				this->exist = true;

				if (right - left >= 0) {
					const char* temp = builder.Divide(right - left);
					if (right - left > 0) {
						_m_str.emplace_back(temp, right - left);
						builder.LeftShift(right - left + 1);
					}
					else {
						builder.LeftShift(1);
					}
				}

				i = i + separator[_select].size() - 1;

				left = i + 1;
				right = left;
			}
			else if (!pass && i == str.size() - 1) {
				if (right - left + 1 > 0) {
					_m_str.emplace_back(builder.Divide(right - left + 1), right - left + 1);
					builder.LeftShift(right - left + 2);
				}
			}
		}
		//cout << "str is " << str <<  " state  is " << state << endl;
	}
public:
	explicit StringTokenizer() : count(0), exist(false), option(0) { }
	explicit StringTokenizer(const string& str, const string& separator, StringBuilder& builder, int option = 0)
		: count(0), exist(false), option(option)
	{
		vector<string> vec; vec.push_back(separator);
		Init(str, vec, builder);
	}
	explicit StringTokenizer(const string& str, const vector<string>& separator, StringBuilder& builder, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(str, separator, builder);
	}
	explicit StringTokenizer(const string& str, StringBuilder& builder, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(str, whitespaces, builder);
	}
	explicit StringTokenizer(string&& str, StringBuilder& builder, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(std::move(str), whitespaces, builder);
	}
	int countTokens()const
	{
		return _m_str.size();
	}
	string nextToken()
	{
		if (hasMoreTokens())
			return _m_str[count++];
		else
			throw "error in nextToken!";
	}
	bool hasMoreTokens()const
	{
		return count < _m_str.size();
	}

	bool isFindExist()const
	{
		return exist;
	}

};
const vector<string> StringTokenizer::whitespaces = { " ", "\t", "\r", "\n" };

class StringTokenizer2
{
private:
	vector<string> _m_str;
	int count;
	bool exist;
	static const vector<string> whitespaces;
	int option;
private:
	void Init(string str, const vector<string>& separator) // assumtion : separators are sorted by length?, long -> short
	{
		if (separator.empty() || str.empty()) { return; } // if str.empty() == false then _m_str.push_back(str); // ?

		int left = 0;
		int right = 0;
		int state = 0;
		this->exist = false;

		for (int i = 0; i < str.size(); ++i) {
			right = i;
			int _select = -1;
			bool pass = false;

			if (1 == option && 0 == state && '\"' == str[i]) {
				if (i == 0) {
					state = 1;
					continue;
				}
				else if (i > 0 && '\\' == str[i - 1]) {
					throw "ERROR for Init on StringTokenizer"; // error!
				}
				else if (i > 0) {
					state = 1;
					continue;
				}
			}
			else if (1 == option && 1 == state && '\"' == str[i]) {
				if ('\\' == str[i - 1]) {
					continue;
				}
				else {
					state = 0;
				}
			}
			else if (1 == option && 1 == state) {
				continue;
			}


			for (int j = 0; j < separator.size(); ++j) {
				for (int k = 0; k < separator[j].size() && i + k < str.size(); ++k) {
					if (str[i + k] == separator[j][k]) {
						pass = true;
					}
					else {
						pass = false;
						break;
					}
				}
				if (pass) { _select = j; break; }
			}

			if (pass) {
				this->exist = true;

				if (right - 1 - left + 1 > 0) {
					_m_str.push_back(str.substr(left, right - 1 - left + 1));
				}
				i = i + separator[_select].size() - 1;
				left = i + 1;
				right = left;
			}
			else if (!pass && i == str.size() - 1) {
				if (right - left + 1 > 0) {
					_m_str.push_back(str.substr(left, right - left + 1));
				}
			}
		}
		//cout << "str is " << str <<  " state  is " << state << endl;
	}
public:
	explicit StringTokenizer2() : count(0), exist(false), option(0) { }
	explicit StringTokenizer2(const string& str, const string& separator, int option = 0)
		: count(0), exist(false), option(option)
	{
		vector<string> vec; vec.push_back(separator);
		Init(str, vec);
	}
	explicit StringTokenizer2(const string& str, const vector<string>& separator, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(str, separator);
	}
	explicit StringTokenizer2(const string& str, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(str, whitespaces);
	}
	explicit StringTokenizer2(string&& str, int option = 0)
		: count(0), exist(false), option(option)
	{
		Init(std::move(str), whitespaces);
	}
	int countTokens()const
	{
		return _m_str.size();
	}
	string nextToken()
	{
		if (hasMoreTokens())
			return _m_str[count++];
		else
			throw "error in nextToken!";
	}
	bool hasMoreTokens()const
	{
		return count < _m_str.size();
	}

	bool isFindExist()const
	{
		return exist;
	}

};
const vector<string> StringTokenizer2::whitespaces = { " ", "\t", "\r", "\n" };

void test1()
{
	const long long SIZE = 100000;
	const string text = "ABC/DEF/GHI/JKL/MNO/PQR/STU//VWX/YZABC/DEF/GHI/JKL/MNO/PQR/STU/VWX/YZ/";
	int a, b;
	a = clock();
	{
		for (int i = 0; i < SIZE; ++i) {

			StringTokenizer2 tokenizer2(text, "/");

			//while (tokenizer2.hasMoreTokens()) {
			//	cout << tokenizer2.nextToken() << " ";
			//}
			//cout << endl;
		}
	}
	b = clock();
	std::cout << b - a << "ms" << endl;

	a = clock();

	{
		StringBuilder builder(102400);
		for (int i = 0; i < SIZE; ++i) {
			StringTokenizer tokenizer1(text, "/", builder);

			//while (tokenizer1.hasMoreTokens()) {
			//	cout << tokenizer1.nextToken() << " ";
			//}
			//cout << endl;
		}
	}
	b = clock();
	std::cout << b - a << "ms" << endl;
}
void test2()
{
	const int SIZE = 1000000;
	{	
		//WizStrBuilder builder;
		StringBuilder builder(102400, "");
		string _A = "wow                         ";
		string _B = " Hello                      ";

		int a, b;
		string C;
		a = clock();
		for (int t = 0; t < SIZE; ++t) {
			builder.Clear();
			builder.Append(_A.c_str(), _A.size());
			builder.Append(_B.c_str(), _B.size());
			builder.Append(_A.c_str(), _A.size());
			builder.Append(_B.c_str(), _B.size());
			
			C = builder.Str(); //Divide(3);
		}
		b = clock();

		cout << b - a << "ms" << endl;

		cout << C << endl;
	}

	{
		int a, b;
		string C;

		a = clock();
		for (int t = 0; t < SIZE; ++t) {
			string A = "wow                         ";
			string B = " Hello                      ";

			C = A + B + A + B;
		}
		b = clock();

		cout << b - a << "ms" << endl;
		cout << C << endl;
	}
}


int main(void)
{
	// test
	test1();

	return 0;
}
