#ifndef HUYS_SHA_H
#define HUYS_SHA_H

#include "ulib.h"

/* 
 *  This structure will hold context information for the hashing
 *  operation
 */
typedef struct SHA1Context
{
    unsigned Message_Digest[5]; /* Message Digest (output)          */

    unsigned Length_Low;        /* Message length in bits           */
    unsigned Length_High;       /* Message length in bits           */

    unsigned char Message_Block[64]; /* 512-bit message blocks      */
    int Message_Block_Index;    /* Index into message block array   */

    int Computed;               /* Is the digest computed?          */
    int Corrupted;              /* Is the message digest corruped?  */
} SHA1Context;

namespace huys
{

namespace crypto
{

class ULIB_API SHA_1
{
public:
	SHA_1();

	int digest(const char * msg);
		
	unsigned int md(int i);
private:
	struct SHA1Context sha;
};

} // namespace crypto

} // namespace huys

#endif // HUYS_SHA_H