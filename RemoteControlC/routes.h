void fix_slashes(const char *inStr, char *outStr);
int check_file_exists(const char *sDir, const char *fileName);
int begin_request_handler(struct mg_connection *conn);

int get_move(struct mg_connection *conn, const struct mg_request_info *ri);
int get_left_click(struct mg_connection *conn, const struct mg_request_info *ri);
int get_right_click(struct mg_connection *conn, const struct mg_request_info *ri);