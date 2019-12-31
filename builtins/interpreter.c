// C\ #include <string.h>
// C\ static char tib[1024], pad[256];
// C{ tib } CONSTANT TIB ( tib )
// C{ COUNT(tib) } CONSTANT /TIB ( per_tib )
// C{ pad } CONSTANT PAD ( pad )
// C{ COUNT(pad) } CONSTANT /PAD ( per_pad )

// C\ static char *source = tib;
// C{ &source } CONSTANT >SOURCE ( to_source )
// C\ static size_t sourcelen = 0;
// C{ &sourcelen } CONSTANT /SOURCE ( per_source )
// C\ static size_t in = 0;
// C{ &in } CONSTANT >IN ( in )

refill_code: /*: REFILL ( refill ) ;*/
	ASMLABEL(refill_code);
	PUSH(sp) = tos;
	if (source != tib) {
		tos = 0;
	} else if ((tos = -!feof(stdin))) {
		int c, i = 0;
		while (!feof(stdin) && (c = fgetc(stdin)) != '\n')
			tib[i++] = c;
		sourcelen = i;
		in = 0;
	}
	NEXT();

word_code: /*: WORD ( word ) ;*/
	ASMLABEL(word_code);
	do {
		size_t i = 0;
		if (tos == ' ') {
			// Treat space and control characters as delimiters
			while (in < sourcelen && source[in] <= ' ')
				in++;
			while (in < sourcelen && source[in] > ' ')
				pad[++i] = source[in++];
		} else {
			// Treat only character @ TOS as delimiter
			while (in < sourcelen && source[in] == tos)
				in++;
			while (in < sourcelen && source[in] != tos)
				pad[++i] = source[in++];
		}
		in++;
		pad[0] = i;
		tos = (cell_t)pad;
	} while (0);
	NEXT();

count_code: /*: COUNT ( count ) ;*/
	ASMLABEL(count_code);
	PUSH(sp) = tos + 1;
	tos = *(char *)tos;
	NEXT();

type_code: /*: TYPE ( type ) ;*/
	ASMLABEL(type_code);
	fwrite((char *)POP(sp), sizeof(char), tos, stdout);
	tos = POP(sp);
	NEXT();

// C\ static struct fthdef forth_wordlist_def;
// C\ static struct fthdef *context[16] = {&forth_wordlist_def};
// C{ context } CONSTANT CONTEXT ( context )
// C\ static size_t order = 1;
// C{ &order } CONSTANT #ORDER ( order )

find_code: /*: FIND ( find ) ;*/
	ASMLABEL(find_code);
	do {
		char *name = (char *)tos + 1;
		int len = ((char *)tos)[0];
		struct fthdef *link = NULL;
		for (int i = order - 1; i >= 0; i--)
			for (link = context[i]; link; link = link->prev) {
				if (link->namelen != len)
					continue;
				if (!strncmp(name, link->name, len))
					goto found;
			}
		PUSH(sp) = tos;
		tos = 0;
		break;
found:
		PUSH(sp) = (cell_t)&link->cf;
		tos = link->namelen & MSB ? 1 : -1;
	} while (0);
	NEXT();

// C\ static int base = 10;
// C{ &base } CONSTANT BASE ( base )

to_number_code: /*: >NUMBER ( to_number ) ;*/
	ASMLABEL(to_number_code);
	do {
		char *s = (char *)sp[-1];
		dcell_t n = *(dcell_t *)&sp[-3];
		while (tos > 0) {
			int d = *s;
			if (d > 'a')
				d -= 'a' - 10;
			else if (d > 'A')
				d -= 'A' - 10;
			else
				d -= '0';
			if (d < 0 || d >= base)
				break;
			n = n * base + d;
			s++; tos--;
		}
		*(dcell_t *)&sp[-3] = n;
		sp[-1] = (cell_t)s;
	} while (0);
	NEXT();

/*
: INTERPRET ( interpret )
	BEGIN
		BL WORD
		DUP C@
	WHILE
		FIND IF
			EXECUTE
		ELSE
			COUNT TYPE
			[CHAR] ? EMIT
			EXIT
		THEN
	REPEAT
	DROP
;

: QUIT ( quit )
	BEGIN
		REFILL
	WHILE
		INTERPRET
	REPEAT
	BYE
;
*/