#define OP2(n,a,b,t) \
n##_code: ASMLABEL(n##_code); \
	tos = b((t POP(sp)) a tos); \
	NEXT();
OP2(add,+,,) /*: + ( add ) ;*/
OP2(sub,-,,) /*: - ( sub ) ;*/
OP2(mul,*,,) /*: * ( mul ) ;*/
OP2(div,/,,) /*: / ( div ) ;*/
OP2(mod,%,,) /*: MOD ( mod ) ;*/
OP2(shl,<<,,(ucell_t)) /*: LSHIFT ( shl ) ;*/
OP2(shr,>>,,(ucell_t)) /*: RSHIFT ( shr ) ;*/

OP2(or,|,,) /*: OR ( or ) ;*/
OP2(and,&,,) /*: AND ( and ) ;*/
OP2(xor,^,,) /*: XOR ( xor ) ;*/

OP2(gt,>,-,) /*: > ( gt ) ;*/
OP2(gte,>=,-,) /*: >= ( gte ) ;*/
OP2(lt,<,-,) /*: < ( lt ) ;*/
OP2(lte,<=,-,) /*: <= ( lte ) ;*/
OP2(eq,==,-,) /*: = ( eq ) ;*/
OP2(neq,!=,-,) /*: <> ( neq ) ;*/

divmod_code: /*: /MOD ( divmod ) ;*/
	ASMLABEL(divmod_code);
	do {
		cell_t x = POP(sp);
		PUSH(sp) = x % tos;
		tos = x / tos;
	} while (0);
	NEXT();

min_code: /*: MIN ( min ) ;*/
	ASMLABEL(min_code);
	do {
		cell_t x = POP(sp);
		if (x < tos)
			tos = x;
	} while (0);
	NEXT();

max_code: /*: MAX ( max ) ;*/
	ASMLABEL(max_code);
	do {
		cell_t x = POP(sp);
		if (x > tos)
			tos = x;
	} while (0);
	NEXT();
