#include <parser.h>
#include <lexer.h>

int main(void)
{
    char *sql = "select one, two, three from tableone, tabletwo";
    YY_BUFFER_STATE bp = yy_scan_bytes(sql, sizeof(sql));
}