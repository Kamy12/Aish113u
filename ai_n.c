static unsigned char	fstchar_n[]= {
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0x00, 0x01, 0xff, 0x02, 0x03, 0x04, 0x05,
	 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0x0c,
	 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
	 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
	 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
	 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
	 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34,
	 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
	 0xff, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43,
	 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
	 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53,
	 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61,
	 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	 0xff, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	 0x79, 0x7a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/* 69 byte -3 = 66 byte */
/* 68 byte 544 bit = 7x77 = 539 + 5bit */
unsigned int
encline_n( str, ptr, len)
unsigned char	*str, *ptr;
unsigned int	len;
{
	unsigned int	i, code, bit;
	extern unsigned char	encode_7[], encode_s1[];
	static unsigned char	sftmap[]= { 0, 7, 6, 5, 4, 3, 2, 1 };
	*str++= encode_7[*ptr & 0x7f];
	for( i= 0, len= ((len-1)<<3)/7+1 ; len >= 3 ; i+=2, len-= 2 ){
		if( !(bit= sftmap[i&7]) )
			ptr++;
		code= *ptr   >> bit;
		code|= *++ptr << (8-bit);
		if( bit > 2 )
			code |= *++ptr << (16-bit);
		code &= 0x3fff;
		if( code < 11193 ){
			*str++= encode_7[code/123];
			if( (*str++= encode_7[code%123]) >= 0x80 ){
				if( !(bit= sftmap[((++i)+1)&7]) )
					ptr++;
				code= *ptr >> bit;
				if( bit > 1 )
					code|= *++ptr << (8-bit);
				*str++= encode_s1[code&0x7f];
				len--;
			}
		}else{
			code-= 11193;
			*str++= encode_7[(code/182)+0x5b];
			*str++= encode_s1[code%182];
		}
	}
	if( len ){
		if( !(bit= sftmap[i&7]) )
			ptr++;
		code= *ptr >> bit;
		if( len == 1 ){
			*str++= encode_7[code];
		}else{
			code |= *++ptr << (8-bit);
			if( bit > 3 )
				code |= *++ptr << (16-bit);
			code &= 0x1fff;
			*str++= encode_7[code/91];
			*str++= encode_7[code%91];
		}
	}
	*str= '\0';
}

unsigned int
decline_n( buf, ptr, len )
unsigned char	*buf, *ptr;
unsigned int	len;
{
	extern unsigned char	fstchar_7[], fstchar_s1[];
	unsigned char	*str= buf;
	unsigned int	i, code, data, bit;

	if( (*str= fstchar_7[*ptr++]) == 0xff )
		return	0;

	for( i= 0, len+= (int)ptr ; (int)ptr < len-3 ; i+= 2 ){
		if( (code= fstchar_n[*ptr++]) == 0xff )
			return	0;
		if( code < 91 ){
			if( (data= fstchar_n[*ptr++]) == 0xff )
				return	0;
			code= code*123 + data;
			if( data >= 91 )
				data= fstchar_s1[*ptr++];
			else
				data= 0xff;
		}else{
			if( (data= fstchar_s1[*ptr++]) == 0xff )
				return	0;
			code= (code-91)*182 + data + 11193;
			data= 0xff;
		}
		code &= 0x3fff;
		if( bit= (i&7) )
			*str |= code << (8-bit);
		*++str= code >> bit;
		if( bit < 6 )
			*++str= code >> (8+bit);
		if( data != 0xff ){
			if( bit= (((++i)+1)&7) )
				*str |= data << (8-bit);
			if( bit < 7 )
				*++str= data >> bit;
		}
	}
	if( (int)ptr < len ){
		code= fstchar_7[*ptr++];
		if( (int)ptr == len-1 ){
			code &= 0x3f;
			if( bit= (i&7) )
				*str |= code << (8-bit);
			str++;
			if( bit < 6 )
				*str= code >> bit;
		}else if( (int)ptr == len-2 ){
			code= (code*91+ fstchar_7[*ptr]) & 0x1fff;
			if( bit= (i&7) )
				*str |= code << (8-bit);
			*++str= code >> bit;
			if( bit < 6 )
				*++str= code >> (8+bit);
		}
	}
	return	str-buf;
}
