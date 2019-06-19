// crc.cpp - originally written and placed in the public domain by Wei Dai

#include "pch.h"
#include "config.h"
#include "crc.h"
#include "misc.h"
#include "cpu.h"

NAMESPACE_BEGIN(CryptoPP)

// We set CRYPTOPP_ARM_CRC32_AVAILABLE based on compiler version.
// Android does not provide -march=armv8-a+crc or -march=armv8.1-a+crc, so
// it looks like we will have to disable CRC acceleration of their devices.
#if defined(__ANDROID__)
# undef CRYPTOPP_ARM_CRC32_AVAILABLE
#endif

// crc-simd.cpp
#if (CRYPTOPP_ARM_CRC32_AVAILABLE)
extern void CRC32_Update_ARMV8(const byte *s, size_t n, word32& c);
extern void CRC32C_Update_ARMV8(const byte *s, size_t n, word32& c);
#endif

// crc-simd.cpp
#if (CRYPTOPP_SSE42_AVAILABLE)
extern void CRC32C_Update_SSE42(const byte *s, size_t n, word32& c);
#endif

/* Table of CRC-32's of all single byte values (made by makecrc.c) */
const word32 CRC32::m_tab[] = {
#ifdef IS_LITTLE_ENDIAN
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
#else
	0x00000000L, 0x96300777L, 0x2c610eeeL, 0xba510999L, 0x19c46d07L,
	0x8ff46a70L, 0x35a563e9L, 0xa395649eL, 0x3288db0eL, 0xa4b8dc79L,
	0x1ee9d5e0L, 0x88d9d297L, 0x2b4cb609L, 0xbd7cb17eL, 0x072db8e7L,
	0x911dbf90L, 0x6410b71dL, 0xf220b06aL, 0x4871b9f3L, 0xde41be84L,
	0x7dd4da1aL, 0xebe4dd6dL, 0x51b5d4f4L, 0xc785d383L, 0x56986c13L,
	0xc0a86b64L, 0x7af962fdL, 0xecc9658aL, 0x4f5c0114L, 0xd96c0663L,
	0x633d0ffaL, 0xf50d088dL, 0xc8206e3bL, 0x5e10694cL, 0xe44160d5L,
	0x727167a2L, 0xd1e4033cL, 0x47d4044bL, 0xfd850dd2L, 0x6bb50aa5L,
	0xfaa8b535L, 0x6c98b242L, 0xd6c9bbdbL, 0x40f9bcacL, 0xe36cd832L,
	0x755cdf45L, 0xcf0dd6dcL, 0x593dd1abL, 0xac30d926L, 0x3a00de51L,
	0x8051d7c8L, 0x1661d0bfL, 0xb5f4b421L, 0x23c4b356L, 0x9995bacfL,
	0x0fa5bdb8L, 0x9eb80228L, 0x0888055fL, 0xb2d90cc6L, 0x24e90bb1L,
	0x877c6f2fL, 0x114c6858L, 0xab1d61c1L, 0x3d2d66b6L, 0x9041dc76L,
	0x0671db01L, 0xbc20d298L, 0x2a10d5efL, 0x8985b171L, 0x1fb5b606L,
	0xa5e4bf9fL, 0x33d4b8e8L, 0xa2c90778L, 0x34f9000fL, 0x8ea80996L,
	0x18980ee1L, 0xbb0d6a7fL, 0x2d3d6d08L, 0x976c6491L, 0x015c63e6L,
	0xf4516b6bL, 0x62616c1cL, 0xd8306585L, 0x4e0062f2L, 0xed95066cL,
	0x7ba5011bL, 0xc1f40882L, 0x57c40ff5L, 0xc6d9b065L, 0x50e9b712L,
	0xeab8be8bL, 0x7c88b9fcL, 0xdf1ddd62L, 0x492dda15L, 0xf37cd38cL,
	0x654cd4fbL, 0x5861b24dL, 0xce51b53aL, 0x7400bca3L, 0xe230bbd4L,
	0x41a5df4aL, 0xd795d83dL, 0x6dc4d1a4L, 0xfbf4d6d3L, 0x6ae96943L,
	0xfcd96e34L, 0x468867adL, 0xd0b860daL, 0x732d0444L, 0xe51d0333L,
	0x5f4c0aaaL, 0xc97c0dddL, 0x3c710550L, 0xaa410227L, 0x10100bbeL,
	0x86200cc9L, 0x25b56857L, 0xb3856f20L, 0x09d466b9L, 0x9fe461ceL,
	0x0ef9de5eL, 0x98c9d929L, 0x2298d0b0L, 0xb4a8d7c7L, 0x173db359L,
	0x810db42eL, 0x3b5cbdb7L, 0xad6cbac0L, 0x2083b8edL, 0xb6b3bf9aL,
	0x0ce2b603L, 0x9ad2b174L, 0x3947d5eaL, 0xaf77d29dL, 0x1526db04L,
	0x8316dc73L, 0x120b63e3L, 0x843b6494L, 0x3e6a6d0dL, 0xa85a6a7aL,
	0x0bcf0ee4L, 0x9dff0993L, 0x27ae000aL, 0xb19e077dL, 0x44930ff0L,
	0xd2a30887L, 0x68f2011eL, 0xfec20669L, 0x5d5762f7L, 0xcb676580L,
	0x71366c19L, 0xe7066b6eL, 0x761bd4feL, 0xe02bd389L, 0x5a7ada10L,
	0xcc4add67L, 0x6fdfb9f9L, 0xf9efbe8eL, 0x43beb717L, 0xd58eb060L,
	0xe8a3d6d6L, 0x7e93d1a1L, 0xc4c2d838L, 0x52f2df4fL, 0xf167bbd1L,
	0x6757bca6L, 0xdd06b53fL, 0x4b36b248L, 0xda2b0dd8L, 0x4c1b0aafL,
	0xf64a0336L, 0x607a0441L, 0xc3ef60dfL, 0x55df67a8L, 0xef8e6e31L,
	0x79be6946L, 0x8cb361cbL, 0x1a8366bcL, 0xa0d26f25L, 0x36e26852L,
	0x95770cccL, 0x03470bbbL, 0xb9160222L, 0x2f260555L, 0xbe3bbac5L,
	0x280bbdb2L, 0x925ab42bL, 0x046ab35cL, 0xa7ffd7c2L, 0x31cfd0b5L,
	0x8b9ed92cL, 0x1daede5bL, 0xb0c2649bL, 0x26f263ecL, 0x9ca36a75L,
	0x0a936d02L, 0xa906099cL, 0x3f360eebL, 0x85670772L, 0x13570005L,
	0x824abf95L, 0x147ab8e2L, 0xae2bb17bL, 0x381bb60cL, 0x9b8ed292L,
	0x0dbed5e5L, 0xb7efdc7cL, 0x21dfdb0bL, 0xd4d2d386L, 0x42e2d4f1L,
	0xf8b3dd68L, 0x6e83da1fL, 0xcd16be81L, 0x5b26b9f6L, 0xe177b06fL,
	0x7747b718L, 0xe65a0888L, 0x706a0fffL, 0xca3b0666L, 0x5c0b0111L,
	0xff9e658fL, 0x69ae62f8L, 0xd3ff6b61L, 0x45cf6c16L, 0x78e20aa0L,
	0xeed20dd7L, 0x5483044eL, 0xc2b30339L, 0x612667a7L, 0xf71660d0L,
	0x4d476949L, 0xdb776e3eL, 0x4a6ad1aeL, 0xdc5ad6d9L, 0x660bdf40L,
	0xf03bd837L, 0x53aebca9L, 0xc59ebbdeL, 0x7fcfb247L, 0xe9ffb530L,
	0x1cf2bdbdL, 0x8ac2bacaL, 0x3093b353L, 0xa6a3b424L, 0x0536d0baL,
	0x9306d7cdL, 0x2957de54L, 0xbf67d923L, 0x2e7a66b3L, 0xb84a61c4L,
	0x021b685dL, 0x942b6f2aL, 0x37be0bb4L, 0xa18e0cc3L, 0x1bdf055aL,
	0x8def022dL
#endif
};

CRC32::CRC32()
{
	Reset();
}

void CRC32::Update(const byte *s, size_t n)
{
#if (CRYPTOPP_ARM_CRC32_AVAILABLE)
	if (HasCRC32())
	{
		CRC32_Update_ARMV8(s, n, m_crc);
		return;
	}
#endif

	word32 crc = m_crc;

	for(; !IsAligned<word32>(s) && n > 0; n--)
		crc = m_tab[CRC32_INDEX(crc) ^ *s++] ^ CRC32_SHIFTED(crc);

	while (n >= 4)
	{
		crc ^= *(const word32 *)(void*)s;
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		n -= 4;
		s += 4;
	}

	while (n--)
		crc = m_tab[CRC32_INDEX(crc) ^ *s++] ^ CRC32_SHIFTED(crc);

	m_crc = crc;
}

void CRC32::TruncatedFinal(byte *hash, size_t size)
{
	ThrowIfInvalidTruncatedSize(size);

	m_crc ^= CRC32_NEGL;
	for (size_t i=0; i<size; i++)
		hash[i] = GetCrcByte(i);

	Reset();
}

// Castagnoli CRC32C (iSCSI)

const word32 CRC32C::m_tab[] = {
#ifdef IS_LITTLE_ENDIAN
    0x00000000L, 0xf26b8303L, 0xe13b70f7L, 0x1350f3f4L, 0xc79a971fL,
    0x35f1141cL, 0x26a1e7e8L, 0xd4ca64ebL, 0x8ad958cfL, 0x78b2dbccL,
    0x6be22838L, 0x9989ab3bL, 0x4d43cfd0L, 0xbf284cd3L, 0xac78bf27L,
    0x5e133c24L, 0x105ec76fL, 0xe235446cL, 0xf165b798L, 0x030e349bL,
    0xd7c45070L, 0x25afd373L, 0x36ff2087L, 0xc494a384L, 0x9a879fa0L,
    0x68ec1ca3L, 0x7bbcef57L, 0x89d76c54L, 0x5d1d08bfL, 0xaf768bbcL,
    0xbc267848L, 0x4e4dfb4bL, 0x20bd8edeL, 0xd2d60dddL, 0xc186fe29L,
    0x33ed7d2aL, 0xe72719c1L, 0x154c9ac2L, 0x061c6936L, 0xf477ea35L,
    0xaa64d611L, 0x580f5512L, 0x4b5fa6e6L, 0xb93425e5L, 0x6dfe410eL,
    0x9f95c20dL, 0x8cc531f9L, 0x7eaeb2faL, 0x30e349b1L, 0xc288cab2L,
    0xd1d83946L, 0x23b3ba45L, 0xf779deaeL, 0x05125dadL, 0x1642ae59L,
    0xe4292d5aL, 0xba3a117eL, 0x4851927dL, 0x5b016189L, 0xa96ae28aL,
    0x7da08661L, 0x8fcb0562L, 0x9c9bf696L, 0x6ef07595L, 0x417b1dbcL,
    0xb3109ebfL, 0xa0406d4bL, 0x522bee48L, 0x86e18aa3L, 0x748a09a0L,
    0x67dafa54L, 0x95b17957L, 0xcba24573L, 0x39c9c670L, 0x2a993584L,
    0xd8f2b687L, 0x0c38d26cL, 0xfe53516fL, 0xed03a29bL, 0x1f682198L,
    0x5125dad3L, 0xa34e59d0L, 0xb01eaa24L, 0x42752927L, 0x96bf4dccL,
    0x64d4cecfL, 0x77843d3bL, 0x85efbe38L, 0xdbfc821cL, 0x2997011fL,
    0x3ac7f2ebL, 0xc8ac71e8L, 0x1c661503L, 0xee0d9600L, 0xfd5d65f4L,
    0x0f36e6f7L, 0x61c69362L, 0x93ad1061L, 0x80fde395L, 0x72966096L,
    0xa65c047dL, 0x5437877eL, 0x4767748aL, 0xb50cf789L, 0xeb1fcbadL,
    0x197448aeL, 0x0a24bb5aL, 0xf84f3859L, 0x2c855cb2L, 0xdeeedfb1L,
    0xcdbe2c45L, 0x3fd5af46L, 0x7198540dL, 0x83f3d70eL, 0x90a324faL,
    0x62c8a7f9L, 0xb602c312L, 0x44694011L, 0x5739b3e5L, 0xa55230e6L,
    0xfb410cc2L, 0x092a8fc1L, 0x1a7a7c35L, 0xe811ff36L, 0x3cdb9bddL,
    0xceb018deL, 0xdde0eb2aL, 0x2f8b6829L, 0x82f63b78L, 0x709db87bL,
    0x63cd4b8fL, 0x91a6c88cL, 0x456cac67L, 0xb7072f64L, 0xa457dc90L,
    0x563c5f93L, 0x082f63b7L, 0xfa44e0b4L, 0xe9141340L, 0x1b7f9043L,
    0xcfb5f4a8L, 0x3dde77abL, 0x2e8e845fL, 0xdce5075cL, 0x92a8fc17L,
    0x60c37f14L, 0x73938ce0L, 0x81f80fe3L, 0x55326b08L, 0xa759e80bL,
    0xb4091bffL, 0x466298fcL, 0x1871a4d8L, 0xea1a27dbL, 0xf94ad42fL,
    0x0b21572cL, 0xdfeb33c7L, 0x2d80b0c4L, 0x3ed04330L, 0xccbbc033L,
    0xa24bb5a6L, 0x502036a5L, 0x4370c551L, 0xb11b4652L, 0x65d122b9L,
    0x97baa1baL, 0x84ea524eL, 0x7681d14dL, 0x2892ed69L, 0xdaf96e6aL,
    0xc9a99d9eL, 0x3bc21e9dL, 0xef087a76L, 0x1d63f975L, 0x0e330a81L,
    0xfc588982L, 0xb21572c9L, 0x407ef1caL, 0x532e023eL, 0xa145813dL,
    0x758fe5d6L, 0x87e466d5L, 0x94b49521L, 0x66df1622L, 0x38cc2a06L,
    0xcaa7a905L, 0xd9f75af1L, 0x2b9cd9f2L, 0xff56bd19L, 0x0d3d3e1aL,
    0x1e6dcdeeL, 0xec064eedL, 0xc38d26c4L, 0x31e6a5c7L, 0x22b65633L,
    0xd0ddd530L, 0x0417b1dbL, 0xf67c32d8L, 0xe52cc12cL, 0x1747422fL,
    0x49547e0bL, 0xbb3ffd08L, 0xa86f0efcL, 0x5a048dffL, 0x8ecee914L,
    0x7ca56a17L, 0x6ff599e3L, 0x9d9e1ae0L, 0xd3d3e1abL, 0x21b862a8L,
    0x32e8915cL, 0xc083125fL, 0x144976b4L, 0xe622f5b7L, 0xf5720643L,
    0x07198540L, 0x590ab964L, 0xab613a67L, 0xb831c993L, 0x4a5a4a90L,
    0x9e902e7bL, 0x6cfbad78L, 0x7fab5e8cL, 0x8dc0dd8fL, 0xe330a81aL,
    0x115b2b19L, 0x020bd8edL, 0xf0605beeL, 0x24aa3f05L, 0xd6c1bc06L,
    0xc5914ff2L, 0x37faccf1L, 0x69e9f0d5L, 0x9b8273d6L, 0x88d28022L,
    0x7ab90321L, 0xae7367caL, 0x5c18e4c9L, 0x4f48173dL, 0xbd23943eL,
    0xf36e6f75L, 0x0105ec76L, 0x12551f82L, 0xe03e9c81L, 0x34f4f86aL,
    0xc69f7b69L, 0xd5cf889dL, 0x27a40b9eL, 0x79b737baL, 0x8bdcb4b9L,
    0x988c474dL, 0x6ae7c44eL, 0xbe2da0a5L, 0x4c4623a6L, 0x5f16d052L,
    0xad7d5351L
#else
    0x00000000L, 0x03836bf2L, 0xf7703be1L, 0xf4f35013L, 0x1f979ac7L,
    0x1c14f135L, 0xe8e7a126L, 0xeb64cad4L, 0xcf58d98aL, 0xccdbb278L,
    0x3828e26bL, 0x3bab8999L, 0xd0cf434dL, 0xd34c28bfL, 0x27bf78acL,
    0x243c135eL, 0x6fc75e10L, 0x6c4435e2L, 0x98b765f1L, 0x9b340e03L,
    0x7050c4d7L, 0x73d3af25L, 0x8720ff36L, 0x84a394c4L, 0xa09f879aL,
    0xa31cec68L, 0x57efbc7bL, 0x546cd789L, 0xbf081d5dL, 0xbc8b76afL,
    0x487826bcL, 0x4bfb4d4eL, 0xde8ebd20L, 0xdd0dd6d2L, 0x29fe86c1L,
    0x2a7ded33L, 0xc11927e7L, 0xc29a4c15L, 0x36691c06L, 0x35ea77f4L,
    0x11d664aaL, 0x12550f58L, 0xe6a65f4bL, 0xe52534b9L, 0x0e41fe6dL,
    0x0dc2959fL, 0xf931c58cL, 0xfab2ae7eL, 0xb149e330L, 0xb2ca88c2L,
    0x4639d8d1L, 0x45bab323L, 0xaede79f7L, 0xad5d1205L, 0x59ae4216L,
    0x5a2d29e4L, 0x7e113abaL, 0x7d925148L, 0x8961015bL, 0x8ae26aa9L,
    0x6186a07dL, 0x6205cb8fL, 0x96f69b9cL, 0x9575f06eL, 0xbc1d7b41L,
    0xbf9e10b3L, 0x4b6d40a0L, 0x48ee2b52L, 0xa38ae186L, 0xa0098a74L,
    0x54fada67L, 0x5779b195L, 0x7345a2cbL, 0x70c6c939L, 0x8435992aL,
    0x87b6f2d8L, 0x6cd2380cL, 0x6f5153feL, 0x9ba203edL, 0x9821681fL,
    0xd3da2551L, 0xd0594ea3L, 0x24aa1eb0L, 0x27297542L, 0xcc4dbf96L,
    0xcfced464L, 0x3b3d8477L, 0x38beef85L, 0x1c82fcdbL, 0x1f019729L,
    0xebf2c73aL, 0xe871acc8L, 0x0315661cL, 0x00960deeL, 0xf4655dfdL,
    0xf7e6360fL, 0x6293c661L, 0x6110ad93L, 0x95e3fd80L, 0x96609672L,
    0x7d045ca6L, 0x7e873754L, 0x8a746747L, 0x89f70cb5L, 0xadcb1febL,
    0xae487419L, 0x5abb240aL, 0x59384ff8L, 0xb25c852cL, 0xb1dfeedeL,
    0x452cbecdL, 0x46afd53fL, 0x0d549871L, 0x0ed7f383L, 0xfa24a390L,
    0xf9a7c862L, 0x12c302b6L, 0x11406944L, 0xe5b33957L, 0xe63052a5L,
    0xc20c41fbL, 0xc18f2a09L, 0x357c7a1aL, 0x36ff11e8L, 0xdd9bdb3cL,
    0xde18b0ceL, 0x2aebe0ddL, 0x29688b2fL, 0x783bf682L, 0x7bb89d70L,
    0x8f4bcd63L, 0x8cc8a691L, 0x67ac6c45L, 0x642f07b7L, 0x90dc57a4L,
    0x935f3c56L, 0xb7632f08L, 0xb4e044faL, 0x401314e9L, 0x43907f1bL,
    0xa8f4b5cfL, 0xab77de3dL, 0x5f848e2eL, 0x5c07e5dcL, 0x17fca892L,
    0x147fc360L, 0xe08c9373L, 0xe30ff881L, 0x086b3255L, 0x0be859a7L,
    0xff1b09b4L, 0xfc986246L, 0xd8a47118L, 0xdb271aeaL, 0x2fd44af9L,
    0x2c57210bL, 0xc733ebdfL, 0xc4b0802dL, 0x3043d03eL, 0x33c0bbccL,
    0xa6b54ba2L, 0xa5362050L, 0x51c57043L, 0x52461bb1L, 0xb922d165L,
    0xbaa1ba97L, 0x4e52ea84L, 0x4dd18176L, 0x69ed9228L, 0x6a6ef9daL,
    0x9e9da9c9L, 0x9d1ec23bL, 0x767a08efL, 0x75f9631dL, 0x810a330eL,
    0x828958fcL, 0xc97215b2L, 0xcaf17e40L, 0x3e022e53L, 0x3d8145a1L,
    0xd6e58f75L, 0xd566e487L, 0x2195b494L, 0x2216df66L, 0x062acc38L,
    0x05a9a7caL, 0xf15af7d9L, 0xf2d99c2bL, 0x19bd56ffL, 0x1a3e3d0dL,
    0xeecd6d1eL, 0xed4e06ecL, 0xc4268dc3L, 0xc7a5e631L, 0x3356b622L,
    0x30d5ddd0L, 0xdbb11704L, 0xd8327cf6L, 0x2cc12ce5L, 0x2f424717L,
    0x0b7e5449L, 0x08fd3fbbL, 0xfc0e6fa8L, 0xff8d045aL, 0x14e9ce8eL,
    0x176aa57cL, 0xe399f56fL, 0xe01a9e9dL, 0xabe1d3d3L, 0xa862b821L,
    0x5c91e832L, 0x5f1283c0L, 0xb4764914L, 0xb7f522e6L, 0x430672f5L,
    0x40851907L, 0x64b90a59L, 0x673a61abL, 0x93c931b8L, 0x904a5a4aL,
    0x7b2e909eL, 0x78adfb6cL, 0x8c5eab7fL, 0x8fddc08dL, 0x1aa830e3L,
    0x192b5b11L, 0xedd80b02L, 0xee5b60f0L, 0x053faa24L, 0x06bcc1d6L,
    0xf24f91c5L, 0xf1ccfa37L, 0xd5f0e969L, 0xd673829bL, 0x2280d288L,
    0x2103b97aL, 0xca6773aeL, 0xc9e4185cL, 0x3d17484fL, 0x3e9423bdL,
    0x756f6ef3L, 0x76ec0501L, 0x821f5512L, 0x819c3ee0L, 0x6af8f434L,
    0x697b9fc6L, 0x9d88cfd5L, 0x9e0ba427L, 0xba37b779L, 0xb9b4dc8bL,
    0x4d478c98L, 0x4ec4e76aL, 0xa5a02dbeL, 0xa623464cL, 0x52d0165fL,
    0x51537dadL
#endif
};

CRC32C::CRC32C()
{
	Reset();
}

void CRC32C::Update(const byte *s, size_t n)
{
#if (CRYPTOPP_SSE42_AVAILABLE)
	if (HasSSE42())
	{
		CRC32C_Update_SSE42(s, n, m_crc);
		return;
	}
#elif (CRYPTOPP_ARM_CRC32_AVAILABLE)
	if (HasCRC32())
	{
		CRC32C_Update_ARMV8(s, n, m_crc);
		return;
	}
#endif

	word32 crc = m_crc;

	for(; !IsAligned<word32>(s) && n > 0; n--)
		crc = m_tab[CRC32_INDEX(crc) ^ *s++] ^ CRC32_SHIFTED(crc);

	while (n >= 4)
	{
		crc ^= *(const word32 *)(void*)s;
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		crc = m_tab[CRC32_INDEX(crc)] ^ CRC32_SHIFTED(crc);
		n -= 4;
		s += 4;
	}

	while (n--)
		crc = m_tab[CRC32_INDEX(crc) ^ *s++] ^ CRC32_SHIFTED(crc);

	m_crc = crc;
}

void CRC32C::TruncatedFinal(byte *hash, size_t size)
{
	ThrowIfInvalidTruncatedSize(size);

	m_crc ^= CRC32_NEGL;
	for (size_t i=0; i<size; i++)
		hash[i] = GetCrcByte(i);

	Reset();
}

NAMESPACE_END
