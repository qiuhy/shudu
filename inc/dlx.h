typedef void (*dlx_answer)(int *, int);
void dlx_init(int maxcol, int maxrow, int max_rowitem, dlx_answer fp);
void dlx_add_node(int r, int c);
int dlx_dance(int dep);