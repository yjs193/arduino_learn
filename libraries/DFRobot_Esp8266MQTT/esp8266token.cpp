
#include "esp8266token.h"

#include "esp8266base64.h"
#include "esp8266md5.h"

#include <stdio.h>
#include <string.h>
#include "Arduino.h"


#define MAX_MESSAGE_LENGTH        4096

unsigned long int ft(
    int t,
    unsigned long int x,
    unsigned long int y,
    unsigned long int z
);

void sha12(
    unsigned char *message,
    int message_length,
    unsigned char *digest
);

#ifdef HMAC_DEBUG
debug_out(
    unsigned char *label,
    unsigned char *data,
    int data_length
)

{
    int i, j;
    int num_blocks;
    int block_remainder;
    num_blocks = data_length / 16;
    block_remainder = data_length % 16;
    printf("%s\n", label);

    for(i = 0; i < num_blocks; i++)
    {
        printf("\t");

        for(j = 0; j < 16; j++)
        {
            printf("%02x ", data[j + (i * 16)]);
        }

        printf("\n");
    }

    if(block_remainder > 0)
    {
        printf("\t");

        for(j = 0; j < block_remainder; j++)
        {
            printf("%02x ", data[j + (num_blocks * 16)]);
        }

        printf("\n");
    }
}
#endif

unsigned long int ft(
    int t,
    unsigned long int x,
    unsigned long int y,
    unsigned long int z
)

{
    unsigned long int a, b, c;

    if(t < 20)
    {
        a = x & y;
        b = (~x) &z;
        c = a ^ b;
    }

    else if(t < 40)
    {
        c = x ^ y ^ z;
    }

    else if(t < 60)
    {
        a = x & y;
        b = a ^ (x & z);
        c = b ^ (y & z);
    }

    else if(t < 80)
    {
        c = (x ^ y) ^z;
    }

    return c;
}

unsigned long int k(int t)

{
    unsigned long int c;

    if(t < 20)
    {
        c = 0x5a827999;
    }

    else if(t < 40)
    {
        c = 0x6ed9eba1;
    }

    else if(t < 60)
    {
        c = 0x8f1bbcdc;
    }

    else if(t < 80)
    {
        c = 0xca62c1d6;
    }

    return c;
}

unsigned long int rotr(int bits, unsigned long int a)

{
    unsigned long int c, d, e, f, g;
    c = (0x0001 << bits) - 1;
    d = ~c;
    e = (a & d) >> bits;
    f = (a & c) << (32 - bits);
    g = e | f;
    return (g & 0xffffffff);
}

unsigned long int rotl(int bits, unsigned long int a)

{
    unsigned long int c, d, e, f, g;
    c = (0x0001 << (32 - bits)) - 1;
    d = ~c;
    e = (a & c) << bits;
    f = (a & d) >> (32 - bits);
    g = e | f;
    return (g & 0xffffffff);
}


void sha12(
    unsigned char *message,
    int message_length,
    unsigned char *digest
)

{
    int i;
    int num_blocks;
    int block_remainder;
    int padded_length;
    unsigned long int l;
    unsigned long int t;
    unsigned long int h[5];
    unsigned long int a, b, c, d, e;
    unsigned long int w[80];
    unsigned long int temp;
#ifdef SHA1_DEBUG
    int x, y;
#endif
    padded_length = message_length + 8;
    padded_length = padded_length + 1;
    l = message_length * 8;
    num_blocks = padded_length / 64;
    block_remainder = padded_length % 64;

    if(block_remainder > 0)
    {
        num_blocks++;
    }

    padded_length = padded_length + (64 - block_remainder);
    for(i = message_length; i < (num_blocks * 64); i++)
    {
        message[i] = 0x00;
    }
    message[message_length] = 0x80;
    message[(num_blocks * 64) - 1] = (unsigned char)(l        & 0xff);
    message[(num_blocks * 64) - 2] = (unsigned char)((l >> 8)  & 0xff);
    message[(num_blocks * 64) - 3] = (unsigned char)((l >> 16) & 0xff);
    message[(num_blocks * 64) - 4] = (unsigned char)((l >> 24) & 0xff);
    h[0] = 0x67452301;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
    h[4] = 0xc3d2e1f0;
#ifdef SHA1_DEBUG
    printf("INITIAL message_length = %d\n", message_length);
    printf("INITIAL padded_length = %d\n", padded_length);
    printf("INITIAL num_blocks = %d\n", num_blocks);

    for(x = 0; x < num_blocks; x++)
    {
        printf("\t\t");

        for(y = 0; y < 16; y++)
        {
            printf("%02x ", message[y + (x * 64)]);
        }

        printf("\n");
        printf("\t\t");

        for(y = 0; y < 16; y++)
        {
            printf("%02x ", message[16 + y + (x * 64)]);
        }

        printf("\n");
        printf("\t\t");

        for(y = 0; y < 16; y++)
        {
            printf("%02x ", message[32 + y + (x * 64)]);
        }

        printf("\n");
        printf("\t\t");

        for(y = 0; y < 16; y++)
        {
            printf("%02x ", message[48 + y + (x * 64)]);
        }

        printf("\n");
    }

#endif

    for(i = 0; i < num_blocks; i++)
    {
        /* Prepare the message schedule */
        for(t = 0; t < 80; t++)
        {
            if(t < 16)
            {
                w[t]  = (256 * 256 * 256) * message[(i * 64) + (t * 4)];
                w[t] += (256 * 256) * message[(i * 64) + (t * 4) + 1];
                w[t] += (256) * message[(i * 64) + (t * 4) + 2];
                w[t] +=                 message[(i * 64) + (t * 4) + 3];
            }

            else if(t < 80)
            {
                w[t] = rotl(1, (w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16]));
            }
        }

#ifdef SHA1_DEBUG
        printf("\tW(0) = %08lX \t W(9)  = %08lX \n", w[0], w[8]);
        printf("\tW(1) = %08lX \t W(10) = %08lX \n", w[1], w[9]);
        printf("\tW(2) = %08lX \t W(11) = %08lX \n", w[2], w[10]);
        printf("\tW(3) = %08lX \t W(12) = %08lX \n", w[3], w[11]);
        printf("\tW(4) = %08lX \t W(13) = %08lX \n", w[4], w[12]);
        printf("\tW(5) = %08lX \t W(14) = %08lX \n", w[5], w[13]);
        printf("\tW(6) = %08lX \t W(15) = %08lX \n", w[6], w[14]);
        printf("\tW(7) = %08lX \t W(16) = %08lX \n\n", w[7], w[15]);
#endif
        /* Initialize the five working variables */
        a = h[0];
        b = h[1];
        c = h[2];
        d = h[3];
        e = h[4];

        /* iterate a-e 80 times */

        for(t = 0; t < 80; t++)
        {
            temp = (rotl(5, a) + ft(t, b, c, d)) & 0xffffffff;
            temp = (temp + e) & 0xffffffff;
            temp = (temp + k(t)) & 0xffffffff;
            temp = (temp + w[t]) & 0xffffffff;
            e = d;
            d = c;
            c = rotl(30, b);
            b = a;
            a = temp;
#ifdef SHA1_DEBUG
            printf("t = %2ld\t %08lx, %08lx, %08lx, %08lx, %08lx\n", t, a, b, c, d, e);
#endif
        }

        /* compute the ith intermediate hash value */
#ifdef SHA1_DEBUG
        printf("  +   \t %08lx, %08lx, %08lx, %08lx, %08lx\n", h[0], h[1], h[2], h[3], h[4]);
#endif
        h[0] = (a + h[0]) & 0xffffffff;
        h[1] = (b + h[1]) & 0xffffffff;
        h[2] = (c + h[2]) & 0xffffffff;
        h[3] = (d + h[3]) & 0xffffffff;
        h[4] = (e + h[4]) & 0xffffffff;
#ifdef SHA1_DEBUG
        printf("  =   \t %08lx, %08lx, %08lx, %08lx, %08lx\n", h[0], h[1], h[2], h[3], h[4]);
#endif
    }

    digest[3]  = (unsigned char)(h[0]       & 0xff);
    digest[2]  = (unsigned char)((h[0] >> 8) & 0xff);
    digest[1]  = (unsigned char)((h[0] >> 16) & 0xff);
    digest[0]  = (unsigned char)((h[0] >> 24) & 0xff);
    digest[7]  = (unsigned char)(h[1]       & 0xff);
    digest[6]  = (unsigned char)((h[1] >> 8) & 0xff);
    digest[5]  = (unsigned char)((h[1] >> 16) & 0xff);
    digest[4]  = (unsigned char)((h[1] >> 24) & 0xff);
    digest[11]  = (unsigned char)(h[2]       & 0xff);
    digest[10]  = (unsigned char)((h[2] >> 8) & 0xff);
    digest[9] = (unsigned char)((h[2] >> 16) & 0xff);
    digest[8] = (unsigned char)((h[2] >> 24) & 0xff);
    digest[15] = (unsigned char)(h[3]       & 0xff);
    digest[14] = (unsigned char)((h[3] >> 8) & 0xff);
    digest[13] = (unsigned char)((h[3] >> 16) & 0xff);
    digest[12] = (unsigned char)((h[3] >> 24) & 0xff);
    digest[19] = (unsigned char)(h[4]       & 0xff);
    digest[18] = (unsigned char)((h[4] >> 8) & 0xff);
    digest[17] = (unsigned char)((h[4] >> 16) & 0xff);
    digest[16] = (unsigned char)((h[4] >> 24) & 0xff);
}

void hmac_sha2(
    unsigned char *key,
    int key_length,
    unsigned char *data,
    int data_length,
    unsigned char *digest
)


{
    int b = 64; /* blocksize */
    unsigned char ipad = 0x36;
    unsigned char opad = 0x5c;
    unsigned char k0[64];
    unsigned char k0xorIpad[64];
    unsigned char step7data[64];
    unsigned char step5data[MAX_MESSAGE_LENGTH + 128];
    unsigned char step8data[64 + 20];
    int i;

    for(i = 0; i < 64; i++)
    {
        k0[i] = 0x00;
    }

    if(key_length != b)     /* Step 1 */
    {
        /* Step 2 */
        if(key_length > b)
        {
            sha12(key, key_length, digest);

            for(i = 0; i < 20; i++)
            {
                k0[i] = digest[i];
            }
        }

        else if(key_length < b)   /* Step 3 */
        {
            for(i = 0; i < key_length; i++)
            {
                k0[i] = key[i];
            }
        }
    }

    else
    {
        for(i = 0; i < b; i++)
        {
            k0[i] = key[i];
        }
    }

#ifdef HMAC_DEBUG
    debug_out("k0", k0, 64);
#endif
    /* Step 4 */

    for(i = 0; i < 64; i++)
    {
        k0xorIpad[i] = k0[i] ^ ipad;
    }

#ifdef HMAC_DEBUG
    debug_out("k0 xor ipad", k0xorIpad, 64);
#endif
    /* Step 5 */

    for(i = 0; i < 64; i++)
    {
        step5data[i] = k0xorIpad[i];
    }

    for(i = 0; i < data_length; i++)
    {
        step5data[i + 64] = data[i];
    }

#ifdef HMAC_DEBUG
    debug_out("(k0 xor ipad) || text", step5data, data_length + 64);
#endif
    /* Step 6 */
    sha12(step5data, data_length + b, digest);
#ifdef HMAC_DEBUG
    debug_out("Hash((k0 xor ipad) || text)", digest, 20);
#endif

    /* Step 7 */

    for(i = 0; i < 64; i++)
    {
        step7data[i] = k0[i] ^ opad;
    }

#ifdef HMAC_DEBUG
    debug_out("(k0 xor opad)", step7data, 64);
#endif

    /* Step 8 */

    for(i = 0; i < 64; i++)
    {
        step8data[i] = step7data[i];
    }

    for(i = 0; i < 20; i++)
    {
        step8data[i + 64] = digest[i];
    }

#ifdef HMAC_DEBUG
    debug_out("(k0 xor opad) || Hash((k0 xor ipad) || text)", step8data, 20 + 64);
#endif
    /* Step 9 */
    sha12(step8data, b + 20, digest);
#ifdef HMAC_DEBUG
    debug_out("HASH((k0 xor opad) || Hash((k0 xor ipad) || text))", digest, 20);
#endif
}

static unsigned char OTA_UrlEncode(char *sign)
{

    char sign_t[40];
    unsigned char i = 0, j = 0;
    unsigned char sign_len = strlen(sign);
    
    if(sign == (void *)0 || sign_len < 28)
        return 1;
    
    for(; i < sign_len; i++)
    {
        sign_t[i] = sign[i];
        sign[i] = 0;
    }
    sign_t[i] = 0;
    
    for(i = 0, j = 0; i < sign_len; i++)
    {
        switch(sign_t[i])
        {
            case '+':
                strcat(sign + j, "%2B");j += 3;
            break;
            
            case ' ':
                strcat(sign + j, "%20");j += 3;
            break;
            
            case '/':
                strcat(sign + j, "%2F");j += 3;
            break;
            
            case '?':
                strcat(sign + j, "%3F");j += 3;
            break;
            
            case '%':
                strcat(sign + j, "%25");j += 3;
            break;
            
            case '#':
                strcat(sign + j, "%23");j += 3;
            break;
            
            case '&':
                strcat(sign + j, "%26");j += 3;
            break;
            
            case '=':
                strcat(sign + j, "%3D");j += 3;
            break;
            
            default:
                sign[j] = sign_t[i];j++;
            break;
        }
    }
    
    sign[j] = 0;
    
    return 0;

}

#define METHOD        "sha1"
unsigned char DF_TOKEN::TOKEN_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *authorization_buf, unsigned short authorization_buf_len)
{
    
    size_t olen = 0;
    
    char sign_buf[40];
    char hmac_sha1_buf[40];
    char access_key_base64[40];
    char string_for_signature[56];
    if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
        || authorization_buf == (void *)0 || authorization_buf_len < 120)
        return 1;

    memset(access_key_base64, 0, sizeof(access_key_base64));
    BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));

    memset(string_for_signature, 0, sizeof(string_for_signature));
    snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);

    memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
    
    hmac_sha2((unsigned char *)access_key_base64, strlen(access_key_base64),
                (unsigned char *)string_for_signature, strlen(string_for_signature),
                (unsigned char *)hmac_sha1_buf);

    olen = 0;
    memset(sign_buf, 0, sizeof(sign_buf));
    BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

    OTA_UrlEncode(sign_buf);
    String buff1 = String(res);
    buff1.replace("/", "%2F");
    String buff2 = String(sign_buf);
    buff2.replace("%", "%25");
    buff2.replace("/", "%2F");
    buff2.replace("+", "%2B");
    buff2.replace(" ", "%20");
    buff2.replace("?", "%3F");
    buff2.replace("#", "%23");
    buff2.replace("&", "%26");
    buff2.replace("=", "%3D");
    snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, buff1.c_str(), et, METHOD, sign_buf);
    return 0;

}

