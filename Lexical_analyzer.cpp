#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<fstream>
using namespace std;
struct character {
	int size;
	int *A;
	int *B;
	int *C;
};
class Lexical_Analyzer {
private:
	int ROWS, COLS;
	int **Array;
	character ch_set;
	int lex_start, cur_state, forward, line_no, lastseen_fstate, remember;

	int ColumnIndex(char c) {
		int index1 = (int)c;
		int index2 = 0;
		for (int i = 0; i < ch_set.size; i++)
			if (ch_set.A[i] == index1) {
				index2 = ch_set.C[i];
				break;
			}
		return index2;
		//return (int)c;
	}
	void Print_Lexeme(int lex_start, int remember, string str) {
		for (int i = lex_start; i <= remember; i++) {
				if (str[i] == '\n')
					cout << "\\n";
				else if (str[i] == '\t')
					cout << "\\t";
				else if (str[i] == '\r')
					cout << "\\r";
				else if (str[i] == '\a')
					cout << "\\a";
				else if (str[i] == '\0')
					cout << "\\0";
				else if (str[i] == '\b')
					cout << "\\b";
				else if (str[i] == '\v')
					cout << "\\v";
				else if (str[i] == '\f')
					cout << "\\f";
				else 
					cout << str[i];
		}
	}
	void split(const string &str, char c, vector<string> &word) {
		stringstream ss(str);
		string item;
		while (getline(ss, item, c))
			word.push_back(item);
	}
public:
	Lexical_Analyzer() {
		this->COLS = 129;
		this->lex_start = this->cur_state = this->forward = this->line_no = 0;
		this->lastseen_fstate = this->remember = -1;
	}
	~Lexical_Analyzer() {
		for (int d = 0; d < ROWS; d++)
			delete[]Array[d];
		this->ROWS = this->COLS = 0;
		delete[]this->ch_set.A;
		delete[]this->ch_set.B;
		delete[]this->ch_set.C;
	}
	void Fill_array() {
		string line_one;
		string line = " ";
		ifstream in("table.txt");
		getline(in, line_one);
		int count = 1;
		while (!in.eof()) {
			getline(in, line);
			if (line.length() > 0)
				count++;
		}
		in.close();

		vector<string> word;
		split(line_one, ' ', word);

		this->ch_set.size = word.size();
		this->ch_set.A = new int[this->ch_set.size];
		this->ch_set.B = new int[this->ch_set.size];
		this->ch_set.C = new int[this->ch_set.size];

		for (int i = 0; i < this->ch_set.size - 1; i++){
			stringstream s(word[i]);
			int num = 0;
			s >> num;
			this->ch_set.A[i] = num;
		}
		for (int j = 0; j < this->ch_set.size - 1; j++) 
			this->ch_set.B[j] = this->ch_set.C[j] = 0;

		this->ch_set.A[ch_set.size - 1] = 128;
		this->ch_set.B[ch_set.size - 1] = 0;
		this->ch_set.C[ch_set.size - 1] = 0;

		this->ROWS = count - 1;
		this->COLS = 129;
		Array = new int*[ROWS];
		for (int y = 0; y < ROWS; y++)
			Array[y] = new int[COLS];
		for (int r = 0; r < ROWS; r++)
			for (int c = 0; c < COLS-1; c++)
				Array[r][c] = -1;
		for (int h = 0; h < ROWS; h++)
			Array[h][COLS - 1] = 0;

		ifstream in2("table.txt");
		getline(in2, line_one);
		int num = 0;
		for (int p = 0; p < ROWS; p++) {
			for (int q = 0; q < ch_set.size; q++) {
				if (!in2.eof())
					in2 >> num;
				this->Array[p][this->ch_set.A[q]] = num;
			}
		}
		in2.close();

	}
	bool check_column(int c1,int c2) {
		for (int i = 0; i < ROWS; i++) {
			if (Array[i][c1] != Array[i][c2])
				return false;
		}
		return true;
	}
	void compact_array() {
		int count = 0;
		for (int i = 0; i < ch_set.size - 1; i++) {
			count++;
			if (ch_set.C[i] == 0) {
				ch_set.C[i] = count;
				ch_set.B[i] = 1;
				for (int j = i + 1; j < ch_set.size - 1; j++) {
					bool same = check_column(ch_set.A[i], ch_set.A[j]);
					if (same == true) 
						if (ch_set.C[j] == 0)
							ch_set.C[j] = count;
				}
			}
			else {
				count--;
			}
		}
		count = count + 2;
		int **temp;
		temp = new int*[ROWS];
		for (int y = 0; y < ROWS; y++)
			temp[y] = new int[count];

		for (int k = 0; k < ROWS; k++) {
			temp[k][0] = -1;
			temp[k][count - 1] = Array[k][COLS-1];
		}

		for (int n = 0; n < ch_set.size - 1; n++) 
			if (ch_set.B[n] == 1) 
				for (int m = 0; m < ROWS; m++) 
					temp[m][ch_set.C[n]] = Array[m][ch_set.A[n]];

		ofstream out("CompactArray.txt");
		for (int g = 0; g < ROWS; g++) {
			for (int h = 0; h < count; h++)
				out << temp[g][h] << "\t";
			out << endl;
		}
		out.close();

		for (int q = 0; q < ROWS; q++)
			delete[]Array[q];
		this->COLS = count;

		Array = new int*[ROWS];
		for (int z = 0; z < ROWS; z++)
			Array[z] = new int[COLS];
		for (int r = 0; r < ROWS; r++)
			for (int c = 0; c < COLS; c++)
				Array[r][c] = temp[r][c];

		for (int f = 0; f < ROWS; f++)
			delete[]temp[f];
	}
	void Lexemes_generator(string str) {
		while (forward < str.length()) {
			int CI;
			char c = '\\';
			if (str[forward] == '\\') {
				forward++;
				if (str[forward] == 'n')
					c = '\n';
				else if (str[forward] == '0')
					c = '\0';
				else if (str[forward] == 't')
					c = '\t';
				else if (str[forward] == 'r')
					c = '\r';
				else if (str[forward] == 'a')
					c = '\a';
				else if (str[forward] == 'b')
					c = '\b';
				else if (str[forward] == 'v')
					c = '\v';
				else if (str[forward] == 'f')
					c = '\f';
				else
					forward--;
				CI = ColumnIndex(c);
			}
			else
				CI = ColumnIndex(str[forward]);
			cur_state = Array[cur_state][CI];
			forward++;
			if (forward >= str.length()) {
				if (cur_state != -1 && Array[cur_state][COLS - 1] != 0) {
					lastseen_fstate = cur_state;
					remember = forward - 1;
					Print_Lexeme(lex_start, remember, str);
					cout << "\t(" << Array[cur_state][COLS - 1] << ")\n";
				}
				else if (cur_state != -1 && Array[cur_state][COLS - 1] == 0)
					if (remember != -1)
						forward = remember + 1;
					else{
						if (lastseen_fstate != -1) {
							Print_Lexeme(lex_start, remember, str);
							cout << "\t(" << Array[lastseen_fstate][COLS - 1] << ")\n";
						}
						else {
							if (remember < lex_start)
								remember = lex_start;
							Print_Lexeme(lex_start, remember, str);
							cout << "\t(Error in line#" << line_no << ")\n";
						}
						lastseen_fstate = -1;
						cur_state = 0;
						forward = remember + 1;
						lex_start = forward;
						remember = -1;
					}
			}
			if (cur_state != -1) {
				if (str[forward] == '\n' || c == '\n')
					line_no++;
				if (Array[cur_state][COLS - 1] != 0) {
					lastseen_fstate = cur_state;
					remember = forward - 1;
				}
			}
			else {
				if (lastseen_fstate != -1) {
					Print_Lexeme(lex_start, remember, str);
					cout << "\t(" << Array[lastseen_fstate][COLS - 1] << ")\n";
				}
				else {
					if (remember < lex_start)
						remember = lex_start;
					Print_Lexeme(lex_start, remember, str);
					cout << "\t(Error in line#" << line_no << ")\n";
				}
				lastseen_fstate = -1;
				cur_state = 0;
				if (str[forward] == '\n' || c == '\n')
					forward = remember + 2;
				forward = remember + 1;
				lex_start = forward;
				remember = -1;
			}
		}
	}
	void diplay_Ch_set(){
		for(int i=0;i<this->ch_set.size;i++)
		{
			cout<<ch_set.A[i]<<"   ";
		}
		cout<<endl;
	}
};
int main() {

	/*ifstream in("input.txt");
	string str;
	getline(in, str,'\0');
	in.close();*/

	string str;
	ifstream in("input.txt");
	stringstream value;
	value << in.rdbuf();
	str = value.str();
	in.close();

	Lexical_Analyzer obj;
	obj.Fill_array();
	obj.compact_array();
	obj.diplay_Ch_set();
	obj.Lexemes_generator(str);

	return 0;
}
