//*********************************************************
//	AES128 encrypt&decrypt

#include <string.h>

#include <stdio.h>
// AES only supports Nb=4
#define Nb 4			// number of columns in the State & expanded key

#define Nk 4			// number of columns in a key
#define Nr 11			// number of rounds in encryption

unsigned char Sbox[256] = {		// forward s-box
0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

unsigned char InvSbox[256] = {	// inverse s-box
0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

// combined Xtimes2[Sbox[]]
unsigned char Xtime2Sbox[256] = {
0xc6, 0xf8, 0xee, 0xf6, 0xff, 0xd6, 0xde, 0x91, 0x60, 0x02, 0xce, 0x56, 0xe7, 0xb5, 0x4d, 0xec, 
0x8f, 0x1f, 0x89, 0xfa, 0xef, 0xb2, 0x8e, 0xfb, 0x41, 0xb3, 0x5f, 0x45, 0x23, 0x53, 0xe4, 0x9b, 
0x75, 0xe1, 0x3d, 0x4c, 0x6c, 0x7e, 0xf5, 0x83, 0x68, 0x51, 0xd1, 0xf9, 0xe2, 0xab, 0x62, 0x2a, 
0x08, 0x95, 0x46, 0x9d, 0x30, 0x37, 0x0a, 0x2f, 0x0e, 0x24, 0x1b, 0xdf, 0xcd, 0x4e, 0x7f, 0xea, 
0x12, 0x1d, 0x58, 0x34, 0x36, 0xdc, 0xb4, 0x5b, 0xa4, 0x76, 0xb7, 0x7d, 0x52, 0xdd, 0x5e, 0x13, 
0xa6, 0xb9, 0x00, 0xc1, 0x40, 0xe3, 0x79, 0xb6, 0xd4, 0x8d, 0x67, 0x72, 0x94, 0x98, 0xb0, 0x85, 
0xbb, 0xc5, 0x4f, 0xed, 0x86, 0x9a, 0x66, 0x11, 0x8a, 0xe9, 0x04, 0xfe, 0xa0, 0x78, 0x25, 0x4b, 
0xa2, 0x5d, 0x80, 0x05, 0x3f, 0x21, 0x70, 0xf1, 0x63, 0x77, 0xaf, 0x42, 0x20, 0xe5, 0xfd, 0xbf, 
0x81, 0x18, 0x26, 0xc3, 0xbe, 0x35, 0x88, 0x2e, 0x93, 0x55, 0xfc, 0x7a, 0xc8, 0xba, 0x32, 0xe6, 
0xc0, 0x19, 0x9e, 0xa3, 0x44, 0x54, 0x3b, 0x0b, 0x8c, 0xc7, 0x6b, 0x28, 0xa7, 0xbc, 0x16, 0xad, 
0xdb, 0x64, 0x74, 0x14, 0x92, 0x0c, 0x48, 0xb8, 0x9f, 0xbd, 0x43, 0xc4, 0x39, 0x31, 0xd3, 0xf2, 
0xd5, 0x8b, 0x6e, 0xda, 0x01, 0xb1, 0x9c, 0x49, 0xd8, 0xac, 0xf3, 0xcf, 0xca, 0xf4, 0x47, 0x10, 
0x6f, 0xf0, 0x4a, 0x5c, 0x38, 0x57, 0x73, 0x97, 0xcb, 0xa1, 0xe8, 0x3e, 0x96, 0x61, 0x0d, 0x0f, 
0xe0, 0x7c, 0x71, 0xcc, 0x90, 0x06, 0xf7, 0x1c, 0xc2, 0x6a, 0xae, 0x69, 0x17, 0x99, 0x3a, 0x27, 
0xd9, 0xeb, 0x2b, 0x22, 0xd2, 0xa9, 0x07, 0x33, 0x2d, 0x3c, 0x15, 0xc9, 0x87, 0xaa, 0x50, 0xa5, 
0x03, 0x59, 0x09, 0x1a, 0x65, 0xd7, 0x84, 0xd0, 0x82, 0x29, 0x5a, 0x1e, 0x7b, 0xa8, 0x6d, 0x2c 
};

// combined Xtimes3[Sbox[]]
unsigned char Xtime3Sbox[256] = {
0xa5, 0x84, 0x99, 0x8d, 0x0d, 0xbd, 0xb1, 0x54, 0x50, 0x03, 0xa9, 0x7d, 0x19, 0x62, 0xe6, 0x9a, 
0x45, 0x9d, 0x40, 0x87, 0x15, 0xeb, 0xc9, 0x0b, 0xec, 0x67, 0xfd, 0xea, 0xbf, 0xf7, 0x96, 0x5b, 
0xc2, 0x1c, 0xae, 0x6a, 0x5a, 0x41, 0x02, 0x4f, 0x5c, 0xf4, 0x34, 0x08, 0x93, 0x73, 0x53, 0x3f, 
0x0c, 0x52, 0x65, 0x5e, 0x28, 0xa1, 0x0f, 0xb5, 0x09, 0x36, 0x9b, 0x3d, 0x26, 0x69, 0xcd, 0x9f, 
0x1b, 0x9e, 0x74, 0x2e, 0x2d, 0xb2, 0xee, 0xfb, 0xf6, 0x4d, 0x61, 0xce, 0x7b, 0x3e, 0x71, 0x97, 
0xf5, 0x68, 0x00, 0x2c, 0x60, 0x1f, 0xc8, 0xed, 0xbe, 0x46, 0xd9, 0x4b, 0xde, 0xd4, 0xe8, 0x4a, 
0x6b, 0x2a, 0xe5, 0x16, 0xc5, 0xd7, 0x55, 0x94, 0xcf, 0x10, 0x06, 0x81, 0xf0, 0x44, 0xba, 0xe3, 
0xf3, 0xfe, 0xc0, 0x8a, 0xad, 0xbc, 0x48, 0x04, 0xdf, 0xc1, 0x75, 0x63, 0x30, 0x1a, 0x0e, 0x6d, 
0x4c, 0x14, 0x35, 0x2f, 0xe1, 0xa2, 0xcc, 0x39, 0x57, 0xf2, 0x82, 0x47, 0xac, 0xe7, 0x2b, 0x95, 
0xa0, 0x98, 0xd1, 0x7f, 0x66, 0x7e, 0xab, 0x83, 0xca, 0x29, 0xd3, 0x3c, 0x79, 0xe2, 0x1d, 0x76, 
0x3b, 0x56, 0x4e, 0x1e, 0xdb, 0x0a, 0x6c, 0xe4, 0x5d, 0x6e, 0xef, 0xa6, 0xa8, 0xa4, 0x37, 0x8b, 
0x32, 0x43, 0x59, 0xb7, 0x8c, 0x64, 0xd2, 0xe0, 0xb4, 0xfa, 0x07, 0x25, 0xaf, 0x8e, 0xe9, 0x18, 
0xd5, 0x88, 0x6f, 0x72, 0x24, 0xf1, 0xc7, 0x51, 0x23, 0x7c, 0x9c, 0x21, 0xdd, 0xdc, 0x86, 0x85, 
0x90, 0x42, 0xc4, 0xaa, 0xd8, 0x05, 0x01, 0x12, 0xa3, 0x5f, 0xf9, 0xd0, 0x91, 0x58, 0x27, 0xb9, 
0x38, 0x13, 0xb3, 0x33, 0xbb, 0x70, 0x89, 0xa7, 0xb6, 0x22, 0x92, 0x20, 0x49, 0xff, 0x78, 0x7a, 
0x8f, 0xf8, 0x80, 0x17, 0xda, 0x31, 0xc6, 0xb8, 0xc3, 0xb0, 0x77, 0x11, 0xcb, 0xfc, 0xd6, 0x3a 
};

unsigned char Xtime2[256] = {
0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 
0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 
0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 
0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 
0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e, 
0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe, 
0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde, 
0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe, 
0x1b, 0x19, 0x1f, 0x1d, 0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05, 
0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d, 0x23, 0x21, 0x27, 0x25, 
0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55, 0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45, 
0x7b, 0x79, 0x7f, 0x7d, 0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65, 
0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d, 0x83, 0x81, 0x87, 0x85, 
0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5, 0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5, 
0xdb, 0xd9, 0xdf, 0xdd, 0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5, 
0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed, 0xe3, 0xe1, 0xe7, 0xe5}; 

unsigned char Xtime9[256] = {
0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77, 
0x90, 0x99, 0x82, 0x8b, 0xb4, 0xbd, 0xa6, 0xaf, 0xd8, 0xd1, 0xca, 0xc3, 0xfc, 0xf5, 0xee, 0xe7, 
0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04, 0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c, 
0xab, 0xa2, 0xb9, 0xb0, 0x8f, 0x86, 0x9d, 0x94, 0xe3, 0xea, 0xf1, 0xf8, 0xc7, 0xce, 0xd5, 0xdc, 
0x76, 0x7f, 0x64, 0x6d, 0x52, 0x5b, 0x40, 0x49, 0x3e, 0x37, 0x2c, 0x25, 0x1a, 0x13, 0x08, 0x01, 
0xe6, 0xef, 0xf4, 0xfd, 0xc2, 0xcb, 0xd0, 0xd9, 0xae, 0xa7, 0xbc, 0xb5, 0x8a, 0x83, 0x98, 0x91, 
0x4d, 0x44, 0x5f, 0x56, 0x69, 0x60, 0x7b, 0x72, 0x05, 0x0c, 0x17, 0x1e, 0x21, 0x28, 0x33, 0x3a, 
0xdd, 0xd4, 0xcf, 0xc6, 0xf9, 0xf0, 0xeb, 0xe2, 0x95, 0x9c, 0x87, 0x8e, 0xb1, 0xb8, 0xa3, 0xaa, 
0xec, 0xe5, 0xfe, 0xf7, 0xc8, 0xc1, 0xda, 0xd3, 0xa4, 0xad, 0xb6, 0xbf, 0x80, 0x89, 0x92, 0x9b, 
0x7c, 0x75, 0x6e, 0x67, 0x58, 0x51, 0x4a, 0x43, 0x34, 0x3d, 0x26, 0x2f, 0x10, 0x19, 0x02, 0x0b, 
0xd7, 0xde, 0xc5, 0xcc, 0xf3, 0xfa, 0xe1, 0xe8, 0x9f, 0x96, 0x8d, 0x84, 0xbb, 0xb2, 0xa9, 0xa0, 
0x47, 0x4e, 0x55, 0x5c, 0x63, 0x6a, 0x71, 0x78, 0x0f, 0x06, 0x1d, 0x14, 0x2b, 0x22, 0x39, 0x30, 
0x9a, 0x93, 0x88, 0x81, 0xbe, 0xb7, 0xac, 0xa5, 0xd2, 0xdb, 0xc0, 0xc9, 0xf6, 0xff, 0xe4, 0xed, 
0x0a, 0x03, 0x18, 0x11, 0x2e, 0x27, 0x3c, 0x35, 0x42, 0x4b, 0x50, 0x59, 0x66, 0x6f, 0x74, 0x7d, 
0xa1, 0xa8, 0xb3, 0xba, 0x85, 0x8c, 0x97, 0x9e, 0xe9, 0xe0, 0xfb, 0xf2, 0xcd, 0xc4, 0xdf, 0xd6, 
0x31, 0x38, 0x23, 0x2a, 0x15, 0x1c, 0x07, 0x0e, 0x79, 0x70, 0x6b, 0x62, 0x5d, 0x54, 0x4f, 0x46};

unsigned char XtimeB[256] = {
0x00, 0x0b, 0x16, 0x1d, 0x2c, 0x27, 0x3a, 0x31, 0x58, 0x53, 0x4e, 0x45, 0x74, 0x7f, 0x62, 0x69, 
0xb0, 0xbb, 0xa6, 0xad, 0x9c, 0x97, 0x8a, 0x81, 0xe8, 0xe3, 0xfe, 0xf5, 0xc4, 0xcf, 0xd2, 0xd9, 
0x7b, 0x70, 0x6d, 0x66, 0x57, 0x5c, 0x41, 0x4a, 0x23, 0x28, 0x35, 0x3e, 0x0f, 0x04, 0x19, 0x12, 
0xcb, 0xc0, 0xdd, 0xd6, 0xe7, 0xec, 0xf1, 0xfa, 0x93, 0x98, 0x85, 0x8e, 0xbf, 0xb4, 0xa9, 0xa2, 
0xf6, 0xfd, 0xe0, 0xeb, 0xda, 0xd1, 0xcc, 0xc7, 0xae, 0xa5, 0xb8, 0xb3, 0x82, 0x89, 0x94, 0x9f, 
0x46, 0x4d, 0x50, 0x5b, 0x6a, 0x61, 0x7c, 0x77, 0x1e, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2f, 
0x8d, 0x86, 0x9b, 0x90, 0xa1, 0xaa, 0xb7, 0xbc, 0xd5, 0xde, 0xc3, 0xc8, 0xf9, 0xf2, 0xef, 0xe4, 
0x3d, 0x36, 0x2b, 0x20, 0x11, 0x1a, 0x07, 0x0c, 0x65, 0x6e, 0x73, 0x78, 0x49, 0x42, 0x5f, 0x54, 
0xf7, 0xfc, 0xe1, 0xea, 0xdb, 0xd0, 0xcd, 0xc6, 0xaf, 0xa4, 0xb9, 0xb2, 0x83, 0x88, 0x95, 0x9e, 
0x47, 0x4c, 0x51, 0x5a, 0x6b, 0x60, 0x7d, 0x76, 0x1f, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2e, 
0x8c, 0x87, 0x9a, 0x91, 0xa0, 0xab, 0xb6, 0xbd, 0xd4, 0xdf, 0xc2, 0xc9, 0xf8, 0xf3, 0xee, 0xe5, 
0x3c, 0x37, 0x2a, 0x21, 0x10, 0x1b, 0x06, 0x0d, 0x64, 0x6f, 0x72, 0x79, 0x48, 0x43, 0x5e, 0x55, 
0x01, 0x0a, 0x17, 0x1c, 0x2d, 0x26, 0x3b, 0x30, 0x59, 0x52, 0x4f, 0x44, 0x75, 0x7e, 0x63, 0x68, 
0xb1, 0xba, 0xa7, 0xac, 0x9d, 0x96, 0x8b, 0x80, 0xe9, 0xe2, 0xff, 0xf4, 0xc5, 0xce, 0xd3, 0xd8, 
0x7a, 0x71, 0x6c, 0x67, 0x56, 0x5d, 0x40, 0x4b, 0x22, 0x29, 0x34, 0x3f, 0x0e, 0x05, 0x18, 0x13, 
0xca, 0xc1, 0xdc, 0xd7, 0xe6, 0xed, 0xf0, 0xfb, 0x92, 0x99, 0x84, 0x8f, 0xbe, 0xb5, 0xa8, 0xa3}; 

unsigned char XtimeD[256] = {
0x00, 0x0d, 0x1a, 0x17, 0x34, 0x39, 0x2e, 0x23, 0x68, 0x65, 0x72, 0x7f, 0x5c, 0x51, 0x46, 0x4b, 
0xd0, 0xdd, 0xca, 0xc7, 0xe4, 0xe9, 0xfe, 0xf3, 0xb8, 0xb5, 0xa2, 0xaf, 0x8c, 0x81, 0x96, 0x9b, 
0xbb, 0xb6, 0xa1, 0xac, 0x8f, 0x82, 0x95, 0x98, 0xd3, 0xde, 0xc9, 0xc4, 0xe7, 0xea, 0xfd, 0xf0, 
0x6b, 0x66, 0x71, 0x7c, 0x5f, 0x52, 0x45, 0x48, 0x03, 0x0e, 0x19, 0x14, 0x37, 0x3a, 0x2d, 0x20, 
0x6d, 0x60, 0x77, 0x7a, 0x59, 0x54, 0x43, 0x4e, 0x05, 0x08, 0x1f, 0x12, 0x31, 0x3c, 0x2b, 0x26, 
0xbd, 0xb0, 0xa7, 0xaa, 0x89, 0x84, 0x93, 0x9e, 0xd5, 0xd8, 0xcf, 0xc2, 0xe1, 0xec, 0xfb, 0xf6, 
0xd6, 0xdb, 0xcc, 0xc1, 0xe2, 0xef, 0xf8, 0xf5, 0xbe, 0xb3, 0xa4, 0xa9, 0x8a, 0x87, 0x90, 0x9d, 
0x06, 0x0b, 0x1c, 0x11, 0x32, 0x3f, 0x28, 0x25, 0x6e, 0x63, 0x74, 0x79, 0x5a, 0x57, 0x40, 0x4d, 
0xda, 0xd7, 0xc0, 0xcd, 0xee, 0xe3, 0xf4, 0xf9, 0xb2, 0xbf, 0xa8, 0xa5, 0x86, 0x8b, 0x9c, 0x91, 
0x0a, 0x07, 0x10, 0x1d, 0x3e, 0x33, 0x24, 0x29, 0x62, 0x6f, 0x78, 0x75, 0x56, 0x5b, 0x4c, 0x41, 
0x61, 0x6c, 0x7b, 0x76, 0x55, 0x58, 0x4f, 0x42, 0x09, 0x04, 0x13, 0x1e, 0x3d, 0x30, 0x27, 0x2a, 
0xb1, 0xbc, 0xab, 0xa6, 0x85, 0x88, 0x9f, 0x92, 0xd9, 0xd4, 0xc3, 0xce, 0xed, 0xe0, 0xf7, 0xfa, 
0xb7, 0xba, 0xad, 0xa0, 0x83, 0x8e, 0x99, 0x94, 0xdf, 0xd2, 0xc5, 0xc8, 0xeb, 0xe6, 0xf1, 0xfc, 
0x67, 0x6a, 0x7d, 0x70, 0x53, 0x5e, 0x49, 0x44, 0x0f, 0x02, 0x15, 0x18, 0x3b, 0x36, 0x21, 0x2c, 
0x0c, 0x01, 0x16, 0x1b, 0x38, 0x35, 0x22, 0x2f, 0x64, 0x69, 0x7e, 0x73, 0x50, 0x5d, 0x4a, 0x47, 
0xdc, 0xd1, 0xc6, 0xcb, 0xe8, 0xe5, 0xf2, 0xff, 0xb4, 0xb9, 0xae, 0xa3, 0x80, 0x8d, 0x9a, 0x97}; 

unsigned char XtimeE[256] = {
0x00, 0x0e, 0x1c, 0x12, 0x38, 0x36, 0x24, 0x2a, 0x70, 0x7e, 0x6c, 0x62, 0x48, 0x46, 0x54, 0x5a, 
0xe0, 0xee, 0xfc, 0xf2, 0xd8, 0xd6, 0xc4, 0xca, 0x90, 0x9e, 0x8c, 0x82, 0xa8, 0xa6, 0xb4, 0xba, 
0xdb, 0xd5, 0xc7, 0xc9, 0xe3, 0xed, 0xff, 0xf1, 0xab, 0xa5, 0xb7, 0xb9, 0x93, 0x9d, 0x8f, 0x81, 
0x3b, 0x35, 0x27, 0x29, 0x03, 0x0d, 0x1f, 0x11, 0x4b, 0x45, 0x57, 0x59, 0x73, 0x7d, 0x6f, 0x61, 
0xad, 0xa3, 0xb1, 0xbf, 0x95, 0x9b, 0x89, 0x87, 0xdd, 0xd3, 0xc1, 0xcf, 0xe5, 0xeb, 0xf9, 0xf7, 
0x4d, 0x43, 0x51, 0x5f, 0x75, 0x7b, 0x69, 0x67, 0x3d, 0x33, 0x21, 0x2f, 0x05, 0x0b, 0x19, 0x17, 
0x76, 0x78, 0x6a, 0x64, 0x4e, 0x40, 0x52, 0x5c, 0x06, 0x08, 0x1a, 0x14, 0x3e, 0x30, 0x22, 0x2c, 
0x96, 0x98, 0x8a, 0x84, 0xae, 0xa0, 0xb2, 0xbc, 0xe6, 0xe8, 0xfa, 0xf4, 0xde, 0xd0, 0xc2, 0xcc, 
0x41, 0x4f, 0x5d, 0x53, 0x79, 0x77, 0x65, 0x6b, 0x31, 0x3f, 0x2d, 0x23, 0x09, 0x07, 0x15, 0x1b, 
0xa1, 0xaf, 0xbd, 0xb3, 0x99, 0x97, 0x85, 0x8b, 0xd1, 0xdf, 0xcd, 0xc3, 0xe9, 0xe7, 0xf5, 0xfb, 
0x9a, 0x94, 0x86, 0x88, 0xa2, 0xac, 0xbe, 0xb0, 0xea, 0xe4, 0xf6, 0xf8, 0xd2, 0xdc, 0xce, 0xc0, 
0x7a, 0x74, 0x66, 0x68, 0x42, 0x4c, 0x5e, 0x50, 0x0a, 0x04, 0x16, 0x18, 0x32, 0x3c, 0x2e, 0x20, 
0xec, 0xe2, 0xf0, 0xfe, 0xd4, 0xda, 0xc8, 0xc6, 0x9c, 0x92, 0x80, 0x8e, 0xa4, 0xaa, 0xb8, 0xb6, 
0x0c, 0x02, 0x10, 0x1e, 0x34, 0x3a, 0x28, 0x26, 0x7c, 0x72, 0x60, 0x6e, 0x44, 0x4a, 0x58, 0x56, 
0x37, 0x39, 0x2b, 0x25, 0x0f, 0x01, 0x13, 0x1d, 0x47, 0x49, 0x5b, 0x55, 0x7f, 0x71, 0x63, 0x6d, 
0xd7, 0xd9, 0xcb, 0xc5, 0xef, 0xe1, 0xf3, 0xfd, 0xa7, 0xa9, 0xbb, 0xb5, 0x9f, 0x91, 0x83, 0x8d}; 

void ShiftRows (unsigned char *State)
{
	unsigned char tmp;

	// just substitute row 0
	State[0] = Sbox[State[0]], State[4] = Sbox[State[4]];
	State[8] = Sbox[State[8]], State[12] = Sbox[State[12]];

	// rotate row 1
	tmp = Sbox[State[1]], State[1] = Sbox[State[5]];
	State[5] = Sbox[State[9]], State[9] = Sbox[State[13]], State[13] = tmp;

	// rotate row 2
	tmp = Sbox[State[2]], State[2] = Sbox[State[10]], State[10] = tmp;
	tmp = Sbox[State[6]], State[6] = Sbox[State[14]], State[14] = tmp;

	// rotate row 3
	tmp = Sbox[State[15]], State[15] = Sbox[State[11]];
	State[11] = Sbox[State[7]], State[7] = Sbox[State[3]], State[3] = tmp;
}


void InvShiftRows (unsigned char *State)
{
	unsigned char tmp;

	// restore row 0
	State[0] = InvSbox[State[0]], State[4] = InvSbox[State[4]];
	State[8] = InvSbox[State[8]], State[12] = InvSbox[State[12]];

	// restore row 1
	tmp = InvSbox[State[13]], State[13] = InvSbox[State[9]];
	State[9] = InvSbox[State[5]], State[5] = InvSbox[State[1]], State[1] = tmp;

	// restore row 2
	tmp = InvSbox[State[2]], State[2] = InvSbox[State[10]], State[10] = tmp;
	tmp = InvSbox[State[6]], State[6] = InvSbox[State[14]], State[14] = tmp;

	// restore row 3
	tmp = InvSbox[State[3]], State[3] = InvSbox[State[7]];
	State[7] = InvSbox[State[11]], State[11] = InvSbox[State[15]], State[15] = tmp;
}

// recombine and mix each row in a column
void MixSubColumns (unsigned char *State)
{
	unsigned char NextState[4 * Nb];
	unsigned char j;
	// mixing column 0
	NextState[0] = Xtime2Sbox[State[0]] ^ Xtime3Sbox[State[5]] ^ Sbox[State[10]] ^ Sbox[State[15]];
	NextState[1] = Sbox[State[0]] ^ Xtime2Sbox[State[5]] ^ Xtime3Sbox[State[10]] ^ Sbox[State[15]];
	NextState[2] = Sbox[State[0]] ^ Sbox[State[5]] ^ Xtime2Sbox[State[10]] ^ Xtime3Sbox[State[15]];
	NextState[3] = Xtime3Sbox[State[0]] ^ Sbox[State[5]] ^ Sbox[State[10]] ^ Xtime2Sbox[State[15]];



	// mixing column 1
	NextState[4] = Xtime2Sbox[State[4]] ^ Xtime3Sbox[State[9]] ^ Sbox[State[14]] ^ Sbox[State[3]];
	NextState[5] = Sbox[State[4]] ^ Xtime2Sbox[State[9]] ^ Xtime3Sbox[State[14]] ^ Sbox[State[3]];
	NextState[6] = Sbox[State[4]] ^ Sbox[State[9]] ^ Xtime2Sbox[State[14]] ^ Xtime3Sbox[State[3]];
	NextState[7] = Xtime3Sbox[State[4]] ^ Sbox[State[9]] ^ Sbox[State[14]] ^ Xtime2Sbox[State[3]];


	// mixing column 2
	NextState[8] = Xtime2Sbox[State[8]] ^ Xtime3Sbox[State[13]] ^ Sbox[State[2]] ^ Sbox[State[7]];
	NextState[9] = Sbox[State[8]] ^ Xtime2Sbox[State[13]] ^ Xtime3Sbox[State[2]] ^ Sbox[State[7]];
	NextState[10]  = Sbox[State[8]] ^ Sbox[State[13]] ^ Xtime2Sbox[State[2]] ^ Xtime3Sbox[State[7]];
	NextState[11]  = Xtime3Sbox[State[8]] ^ Sbox[State[13]] ^ Sbox[State[2]] ^ Xtime2Sbox[State[7]];



	// mixing column 3
	NextState[12] = Xtime2Sbox[State[12]] ^ Xtime3Sbox[State[1]] ^ Sbox[State[6]] ^ Sbox[State[11]];
	NextState[13] = Sbox[State[12]] ^ Xtime2Sbox[State[1]] ^ Xtime3Sbox[State[6]] ^ Sbox[State[11]];
	NextState[14] = Sbox[State[12]] ^ Sbox[State[1]] ^ Xtime2Sbox[State[6]] ^ Xtime3Sbox[State[11]];
	NextState[15] = Xtime3Sbox[State[12]] ^ Sbox[State[1]] ^ Sbox[State[6]] ^ Xtime2Sbox[State[11]];

	for(j=0;j<sizeof(NextState);j++)
		*(State+j)=*(NextState+j);



}

// restore and un-mix each row in a column
void InvMixSubColumns (unsigned char *State)
{
	unsigned char NextState[4 * Nb];
	int i;

	// restore column 0
	NextState[0] = XtimeE[State[0]] ^ XtimeB[State[1]] ^ XtimeD[State[2]] ^ Xtime9[State[3]];
	NextState[5] = Xtime9[State[0]] ^ XtimeE[State[1]] ^ XtimeB[State[2]] ^ XtimeD[State[3]];
	NextState[10] = XtimeD[State[0]] ^ Xtime9[State[1]] ^ XtimeE[State[2]] ^ XtimeB[State[3]];
	NextState[15] = XtimeB[State[0]] ^ XtimeD[State[1]] ^ Xtime9[State[2]] ^ XtimeE[State[3]];

	// restore column 1
	NextState[4] = XtimeE[State[4]] ^ XtimeB[State[5]] ^ XtimeD[State[6]] ^ Xtime9[State[7]];
	NextState[9] = Xtime9[State[4]] ^ XtimeE[State[5]] ^ XtimeB[State[6]] ^ XtimeD[State[7]];
	NextState[14] = XtimeD[State[4]] ^ Xtime9[State[5]] ^ XtimeE[State[6]] ^ XtimeB[State[7]];
	NextState[3] = XtimeB[State[4]] ^ XtimeD[State[5]] ^ Xtime9[State[6]] ^ XtimeE[State[7]];

	// restore column 2
	NextState[8] = XtimeE[State[8]] ^ XtimeB[State[9]] ^ XtimeD[State[10]] ^ Xtime9[State[11]];
	NextState[13] = Xtime9[State[8]] ^ XtimeE[State[9]] ^ XtimeB[State[10]] ^ XtimeD[State[11]];
	NextState[2]  = XtimeD[State[8]] ^ Xtime9[State[9]] ^ XtimeE[State[10]] ^ XtimeB[State[11]];
	NextState[7]  = XtimeB[State[8]] ^ XtimeD[State[9]] ^ Xtime9[State[10]] ^ XtimeE[State[11]];

	// restore column 3
	NextState[12] = XtimeE[State[12]] ^ XtimeB[State[13]] ^ XtimeD[State[14]] ^ Xtime9[State[15]];
	NextState[1] = Xtime9[State[12]] ^ XtimeE[State[13]] ^ XtimeB[State[14]] ^ XtimeD[State[15]];
	NextState[6] = XtimeD[State[12]] ^ Xtime9[State[13]] ^ XtimeE[State[14]] ^ XtimeB[State[15]];
	NextState[11] = XtimeB[State[12]] ^ XtimeD[State[13]] ^ Xtime9[State[14]] ^ XtimeE[State[15]];

	for( i=0; i < 4 * Nb; i++ )
		State[i] = InvSbox[NextState[i]];
}



void AddRoundKey (unsigned *State, unsigned *Key)
{
	int i;

	for( i = 0; i < 4; i++ )
		State[i] ^= Key[i];
}

unsigned char Rcon[22] = {0x35, 0x47, 0x58, 0x6f, 0x7e, 0x40, 0x20, 0x80, 0x1e, 0x26,0x3f,
					      0x35, 0x47, 0x58, 0x6f, 0x7e, 0x11, 0x26, 0x43, 0x81, 0x1b, 0x36};

//unsigned char Rcon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};


// produce Nk bytes for each round
void ExpandKey (unsigned char *key, unsigned char *ExpKey)
{
	unsigned char tmp0, tmp1, tmp2, tmp3, tmp4;
	unsigned idx;

	for( idx = 0; idx < Nk; idx++ )
	{
		ExpKey[4*idx+0] = key[4 * idx + 0];
		ExpKey[4*idx+1] = key[4 * idx + 1];
		ExpKey[4*idx+2] = key[4 * idx + 2];
		ExpKey[4*idx+3] = key[4 * idx + 3];
	}

	for( idx = Nk; idx < Nb * (Nr + 1); idx++ )
	{
		tmp0 = ExpKey[4*idx - 4];
		tmp1 = ExpKey[4*idx - 3];
		tmp2 = ExpKey[4*idx - 2];
		tmp3 = ExpKey[4*idx - 1];
		if( !(idx % Nk) )
		{
			tmp4 = tmp3;
			tmp3 = Sbox[tmp0];
			tmp0 = Sbox[tmp1] ^ Rcon[idx/Nk];
			tmp1 = Sbox[tmp2];
			tmp2 = Sbox[tmp4];
		}

		//  convert from longs to bytes

		ExpKey[4*idx+0] = ExpKey[4*idx - 4*Nk + 0] ^ tmp0;
		ExpKey[4*idx+1] = ExpKey[4*idx - 4*Nk + 1] ^ tmp1;
		ExpKey[4*idx+2] = ExpKey[4*idx - 4*Nk + 2] ^ tmp2;
		ExpKey[4*idx+3] = ExpKey[4*idx - 4*Nk + 3] ^ tmp3;
	}
}

// encrypt one 128 bit block
void Encrypt (unsigned char *InText, unsigned char *ExpKey, unsigned char *OutText)
{
	unsigned round, idx;
	unsigned char State[Nb * 4];

	for( idx = 0; idx < Nb; idx++ )
	{
		State[4*idx+0] = *InText++;
		State[4*idx+1] = *InText++;
		State[4*idx+2] = *InText++;
		State[4*idx+3] = *InText++;
	}

	AddRoundKey ((unsigned *)State, (unsigned *)ExpKey);

	for( round = 1; round < Nr + 1; round++ )
	{
		if( round < Nr )
			MixSubColumns (State);
		else
			ShiftRows (State);

		AddRoundKey ((unsigned *)State, (unsigned *)ExpKey + round * Nb);
	}

	for( idx = 0; idx < Nb; idx++ )
	{
		*OutText++ = State[4*idx+0];
		*OutText++ = State[4*idx+1];
		*OutText++ = State[4*idx+2];
		*OutText++ = State[4*idx+3];
	}
}

void Decrypt (unsigned char *InText, unsigned char *ExpKey, unsigned char *OutText)
{
	unsigned int idx, round;
	unsigned char State[Nb * 4];

	for( idx = 0; idx < Nb; idx++ )
	{
		State[4*idx+0] = *InText++;
		State[4*idx+1] = *InText++;
		State[4*idx+2] = *InText++;
		State[4*idx+3] = *InText++;
	}

	AddRoundKey ((unsigned *)State, (unsigned *)ExpKey + Nr * Nb);
	round = Nr;

	InvShiftRows(State);

	while( round-- )
	{
		AddRoundKey ((unsigned *)State, (unsigned *)ExpKey + round * Nb);
		if( round )
			InvMixSubColumns (State);
	} 

	for( idx = 0; idx < Nb; idx++ )
	{
		*OutText++ = State[4*idx+0];
		*OutText++ = State[4*idx+1];
		*OutText++ = State[4*idx+2];
		*OutText++ = State[4*idx+3];
	}
}

unsigned int EncryptBuf(char *lpInBuf, unsigned int dwDataLen, char *lpOutBuf, char *lpKey16Byte) // out buf len = dwDataLen + 16
{
	unsigned char nExpKey[4*Nb*(Nr+1)];
	int i;
	unsigned int dwPackets;
	char nAlign[16], nRemainder;
	char *lpIn, *lpOut;
	
	ExpandKey(lpKey16Byte,nExpKey);

	dwPackets = dwDataLen / 16;
	nRemainder = dwDataLen % 16;

	lpIn = lpInBuf;
	lpOut = lpOutBuf;
	
	for(i=0; i<dwPackets; i++)
	{
		Encrypt(lpIn,nExpKey,lpOut);
		lpIn += 16;
		lpOut += 16;
	}

	nAlign[15] = nRemainder;
	for(i=0; i<15; i++)
	{
		if(nRemainder > 0)
		{
			nAlign[i] = *lpIn;
			lpIn++;
			nRemainder--;
		}
		else
		{
			nAlign[i] = 0;
		}
	}

	nRemainder = nAlign[15];
	Encrypt(nAlign,nExpKey,lpOut);

	return (dwDataLen + (16 - nRemainder));
}

unsigned int DecryptBuf(char *lpInBuf, unsigned int dwDataLen, char *lpOutBuf, char *lpKey16Byte) // return decrypt data len
{
	unsigned char nExpKey[4*Nb*(Nr+1)];
	int i;
	unsigned int dwPackets;
	char nRemainder;
	char *lpIn, *lpOut;

	if(((dwDataLen % 16) != 0) || (dwDataLen == 0))
	{
		printf("test333\r\n");
		return 0;
	}

	ExpandKey(lpKey16Byte,nExpKey);

	dwPackets = dwDataLen / 16;
	
	lpIn = lpInBuf;
	lpOut = lpOutBuf;
	
	for(i=0; i<dwPackets; i++)
	{
		Decrypt(lpIn,nExpKey,lpOut);
		lpIn += 16;
		lpOut += 16;
	}

	lpOut--;

	nRemainder = *lpOut;
       
	if(nRemainder > 15)
	{
		printf("test 35553:%d\r\n",nRemainder);
		return 0;
	}

	return (dwDataLen + nRemainder - 16);
}

unsigned int JSHash(unsigned int dwHash, char *lpData, int len)
{  
	unsigned int hash;
	int i;
	if(dwHash == 0)
	{
		hash = 1315321911;
	}
	else
	{
		hash = dwHash;
	}
	for(i=0; i<len; i++)
	{
		hash ^= ((hash << 5) + lpData[i] + (hash >> 2));  
	}
	return hash;
}  

void HMMakeKey(char *lpRandom256, char *lpKey16) // in: Random 256byte, out:key 16byte
{
	unsigned int lpKey[4];

	//lpKey = (unsigned int *)lpKey16;
	lpKey[0] = JSHash(0, lpRandom256, 256);
	lpKey[1] = JSHash(lpKey[0], lpRandom256, 256);
	lpKey[2] = JSHash(lpKey[1], lpRandom256, 256);
	lpKey[3] = JSHash(lpKey[2], lpRandom256, 256);
	memcpy(lpKey16,(unsigned char *)lpKey,16);
}

void user_make_aes_key(char *key16)
{
	unsigned int rand;

	//HMMakeKey(rand, key16);
}

/*void main_test()
{
	#define DATA_LEN 47

	unsigned char in[DATA_LEN]=
	{
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
	};

	unsigned char rand[256] = 
	{
		0x33,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
		0x32,0x43,0xf6,0xad,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x06,0x33
	};
	
	unsigned char key1[16];

	unsigned char OutText[DATA_LEN + 16];
	unsigned char OutText2[DATA_LEN + 16];
	unsigned char i;
	unsigned int len;

	HMMakeKey(rand, key1);

	printf("HM Make Key:\r\n");
	for(i=0;i<16;i++)
	{
		printf("0x%02x ",key1[i]);
	 
	}
	printf("\r\n");

	len = EncryptBuf(in, DATA_LEN, OutText, key1);
	
	printf("encrypt in:len=%d\r\n", DATA_LEN);
	for(i=0;i<DATA_LEN;i++)
	{
		printf("0x%02x ",in[i]);
	 
	}
	
	printf("\r\n");
	printf("encrypt len=%d\r\n", len);
	
	for(i=0;i<len;i++)
	{
		printf("0x%02x ",OutText[i]);
	
	}
	
	printf("\r\n");

	len = DecryptBuf(OutText, len, OutText2, key1);
	
	printf("Decrypt len=%d\r\n", len);
	for(i=0;i<len;i++)
	{
		printf("0x%02x ",OutText2[i]);
	}
	printf("\r\n");
}*/

