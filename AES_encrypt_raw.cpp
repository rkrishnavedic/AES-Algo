//Not working properly, it needs more debugging...

#include<iostream>
#include<string>


using namespace std;

#define int uint8_t

#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))

//source code : https://en.wikipedia.org/wiki/Rijndael_S-box
class sbox{
    
public:
uint8_t sbox_matrix[256];
uint8_t sbox_inverse[256];
    
    
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
       for(int i=0;i<=255;i++){
           sbox_inverse[(int)sbox_lookup(i)]=(uint8_t)i;
       }
   }
     uint8_t sbox_inv_lookup(uint8_t value){
            return sbox_inverse[value];
   }
    ~sbox(){
        //destructor of sbox as we have to just import the data;
    }
};


class AES_128_encrypt{

    int rounds=10;
    /*
     10 rounds in 128bit encryption
     12 rounds in 196bit encryption
     14 rounds in 256bit encryption
     */
    int N=4;
    /*
     N as the length of the key in 32-bit words: 4 words for AES-128,
     6 words for AES-192, and
     8 words for AES-256
     */
    uint8_t encrypt_box[16];
    uint8_t original_sbox[256];
    uint8_t inverse_sbox[256];
    
    uint8_t round_const[11]={0x00, 0x01, 0x02, 0x04 , 0x08 ,0x10, 0x20 ,0x40, 0x80, 0x1B,0x36};
    
    uint8_t round_key[40][4];

public:
    
    void import_sbox(uint8_t sbox_generated[], uint8_t sbox_inverse[]){
        for(int i=0;i<=255;i++){
            original_sbox[i]=(uint8_t)sbox_generated[i];
            inverse_sbox[i]=(uint8_t)sbox_inverse[i];
        }
    }
    
    void add_encryption_key(uint8_t key_value[16]){
        for(int row=0;row<4;row++){
            for(int col=0;col<4;col++){
                round_key[row][col]=key_value[row*4+col];
            }
        }
    }

    void subBytes(uint8_t plainbox[32]){
            // storing the plain32_data to encrypt_box (byte)
            for(int i=0;i<32;i++){
                encrypt_box[i]=plainbox[i];
            }
            // substitution with the Rijndael SBOX-256(16x16 original_table for encryption)
            for(int i=0;i<32;i++){
                //transformation- substitution
                //index typecast to int from uint8_t
                //encrypt_box[i] = original_sbox[(int)encrypt_box[i]];
            }
    }

    void shift_rows(){
        for(int row=1;row<4;row++){
            //row, col=column
            //row 0 has no shift so we skip that
            /*??shift the array in-place by k steps (will improve later)
            */
            int temporary_var;
            for(int shift_count=1;shift_count<=row;shift_count++){
                temporary_var=encrypt_box[row*4];
                for(int col=1;col<4;col++){
                     encrypt_box[row*4 +col-1]=encrypt_box[row*4+col];
                }
                encrypt_box[row*4+3]=temporary_var;
            }
        }
    }

    void mixColumns_transformation(int* input_mixcol){
        /*
        transformation Galois Field
         source:
        https://en.wikipedia.org/wiki/Rijndael_MixColumns
        */
    //int ans[4];
    uint8_t a[4];
    uint8_t b[4];
    uint8_t c;
    uint8_t h;
     /* The array 'a' is simply a copy of the input array 'input_mixcol'
       * The array 'b' is each element of the array 'a' multiplied by 2
       * in Rijndael's Galois field
       * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
     for (c = 0; c < 4; c++) {
         a[c] = (uint8_t)input_mixcol[c];
         /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
         h =(input_mixcol[c] >> 7); /* arithmetic right shift, thus shifting in either zeros or ones */
         b[c] = (uint8_t)((int)input_mixcol[c] << 1); /* implicitly removes high bit because b[c] is an 8-bit uint8_t, so we xor by 0x1b and not 0x11b in the next line */
         b[c] ^= 0x1B & h; /* Rijndael's Galois field */
     }
     input_mixcol[0] = (uint8_t)(b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]); /* 2 * a0 + a3 + a2 + 3 * a1 */
     input_mixcol[1] = (uint8_t)(b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]); /* 2 * a1 + a0 + a3 + 3 * a2 */
     input_mixcol[2] = (uint8_t)(b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]); /* 2 * a2 + a1 + a0 + 3 * a3 */
     input_mixcol[3] = (uint8_t)(b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]); /* 2 * a3 + a2 + a1 + 3 * a0 */
     
    }

    void mixColumns(){
        
        int temp_vector[4];
        
            for(int col=0;col<4;col++){
                 for(int row=0;row<4;row++){
                     temp_vector[row]=encrypt_box[row*4+col];
            }
                //cout<<temp_vector<<"::debug"<<endl;
                mixColumns_transformation(temp_vector);
                //cout<<temp_vector<<"::debug"<<endl;
           for(int row=0;row<4;row++){
                     encrypt_box[row*4+col]=temp_vector[row];
            }
        }
        
    }
    
    void calculate_round_key(){
        
        for(int i=N;i<N*rounds;i++){
            if(i%N==0){
                
                //W_{i-N} xor SubByte( Rotate_one_left_cycle_top_to_bottom (W_{i-1})) xor round_const[i/N]
                
                for(int row=0;row<4;row++){
                    round_key[i][row]=(round_key[i-N][row] ^ original_sbox[round_key[i][(row+1)%4]]) ^ round_const[i/N];
                }
                
                
            }else{
                //W_{i-N} xor W_{i-1}
                
                             for(int row=0;row<4;row++){
                                 round_key[i][row]=(round_key[i-N][row] ^ round_key[i-1][row]);
                             }
            }
        }
    }
    
    void addRoundKey(){
        
        for(int iterate_rounds=0;iterate_rounds<rounds;iterate_rounds++){
            
            for(int row=0;row<4;row++){
                for(int col=0;col<4;col++){
                    encrypt_box[row*4+col]=(encrypt_box[row*4+col] ^ round_key[iterate_rounds][row*4+col]);
                }
            }
        }
        
    }

    void show(){
        for(int row=0;row<4;row++){
            for(int col=0;col<4;col++){
                cout<<encrypt_box[row*4+col]<<", ";
            }
            cout<<endl;
        }
    }

};

int32_t main(){
    
    sbox temporary_sbox;
    temporary_sbox.initialize_sbox();
    temporary_sbox.initialize_inverse();
    

    AES_128_encrypt aes_algo;
    //import the sbox;
    aes_algo.import_sbox(temporary_sbox.sbox_matrix, temporary_sbox.sbox_inverse);
    
    //calling temporary_sbox destructor explicitely as we don't require it!
    temporary_sbox.~sbox();
    
    


        string s;
        s="yuten-nd_s jaf; afjfjajfi;a nifaj ija ;f jaiefj eiajfjfka fkdjfla;fj ade & $$$ djf a** ==\n";
    uint8_t key_en[16];
    string good_key ="9syiwfehoh9efuw392";
    
    
uint8_t plaindata[16];
        for(int i=0;i<16;i++){
plaindata[i]=(s[i]);
            key_en[i]=good_key[i];
        }
       
    aes_algo.subBytes(plaindata);
    
    aes_algo.add_encryption_key(key_en);
    aes_algo.show();

    aes_algo.shift_rows();
    cout<<"Shifted rows"<<endl;
    aes_algo.show();


    aes_algo.mixColumns();
    cout<<"mixed Cols"<<endl;
    aes_algo.show();
    aes_algo.calculate_round_key();

    aes_algo.addRoundKey();

    cout<<"added round key"<<endl;
    aes_algo.show();
    
    cout<<"removed round key"<<endl;
    aes_algo.show();
    
    
    
    
    
    
    return 0;
}
