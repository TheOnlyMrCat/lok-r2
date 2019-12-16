/* Currently, these are in no particular order.
 * When released, the order will be normalised.
 */

enum NodeType {
    NONE,
    LOAD,
    RUN,
    FUNCDEF,
    BLOCK,
    TYPESINGLE,
    TYPEMULTI,
    TYPEFN,
    PARAMLIST,
    PARAM,
    DECL,
    LIBNAME,
    FILEPATH,
    QUALID,
    QUALIDOP,
    QUALPART,
    EXPRASSIG,
    EXPRBASIC,
    EXPRDECL,
    VALINT,
    VALFLOAT,
    VALSTR
}