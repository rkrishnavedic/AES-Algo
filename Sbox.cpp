#include <iostream>
#include <string>

using namespace std;
#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

char hex_char(int a){
    if(a%16<10) return a%16+'0';
    int after_value=a%16;
    if(after_value==10) return 'a';
    if(after_value==11) return 'b';
    if(after_value==12) return 'c';
    if(after_value==13) return 'd';
    if(after_value==14) return 'e';
    return 'f';
}

string hex_value(uint8_t type_value){
string ans="00";
int value=(int)type_value;
ans[1]=hex_char(value%16);
ans[0]=hex_char((value/16)%16);

return ans;
}

//source code : https://en.wikipedia.org/wiki/Rijndael_S-box
class sbox{
uint8_t sbox_matrix[256];
uint8_t sbox_inverse[256];
public:
   void initialize_sbox(){
       uint8_t p = 1, q = 1;
	do {
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;
		uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

		sbox_matrix[p] = xformed ^ 0x63;
	} while (p != 1);
	sbox_matrix[0] = 0x63;
   }

   uint8_t sbox_lookup(uint8_t value){
            return sbox_matrix[value];
   }
   void initialize_inverse(){
       for(int i=0;i<256;i++){
           sbox_inverse[(int)sbox_lookup(i)]=(uint8_t)i;
       }
   }
     uint8_t sbox_inv_lookup(uint8_t value){
            return sbox_inverse[value];
   }
};

int32_t main(){

    string plain_text;
    cin>>plain_text;

    sbox Table;
    Table.initialize_sbox();
    Table.initialize_inverse();

    cout<<"This is AES-SBOX"<<endl;

         for(int i=0;i<16;i++){
            for(int j=0;j<16;j++){
                cout<<hex_value(Table.sbox_lookup((uint8_t)(16*i+j)))<<" ";
            }
            cout<<endl;
        }

        cout<<endl<<"This is Inverse-SBOX"<<endl;

        for(int i=0;i<16;i++){
            for(int j=0;j<16;j++){
                cout<<hex_value(Table.sbox_inv_lookup((uint8_t)(16*i+j)))<<" ";
            }
            cout<<endl;
        }
    

    return 0;
}
