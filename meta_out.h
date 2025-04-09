#ifndef META_OUT_H
#define META_OUT_H
#include <common.h>
#include "epan/packet.h"
#include "epan/epan_dissect.h"
#include <utf8_to_utf16.h>
#include <meta_common.h>
#include <cJSON/cJSON.h>

// коды возврата func
#define PROCESS_IGNORE -1				// если ключ встретился, не обрабатываем его
#define PROCESS_NORMAL 0	// если ключ встретился, обрабатываем его
#define PROCESS_DUP 1		// если ключ встретился, обрабатываем его, но к имени приставляем суффик с номером, чтобы сохранить уникальность имен.
#define PROCESS_INSERT	2			// вставляем строку до вызова рекурсии и/или после
#define PROCESS_PASS	3			// позволяет рекурсивно пройти внутрь блока


#define IS_KEY(fi, key) if (!strcmp(fi->hfinfo->abbrev, key))
#define IS_KEY_INCLUDE(fi, key, n) if (!strncmp(fi->hfinfo->abbrev, key, n))
#define NOT_KEY(fi, key) if (strcmp(fi->hfinfo->abbrev, key))
#define GET_VAL(fi) wrap_fvalue_to_string_repr(NULL, fi->value, FTREPR_DISPLAY, fi->hfinfo->display)
#define FREE_VAL(val) if (val) { wrap_wmem_free(NULL, val); val=NULL; }
#define GET_UINT(fi) fvalue_get_uinteger(fi->value)
#define GET_UINT64(fi) fvalue_get_uinteger64(fi->value)
#define IS_TEXT(fi , text) if (!strncmp(fi->rep->representation, text, sizeof(text)-1))

#define PROTONAME_MAX_LEN 20		// макс. длина имени протокола
typedef struct {
    task_t *task;
    uint32_t src;							// из IPsort, если он есть
    uint32_t dst;							// из IPsort, если он есть
    GByteArray *passport;
    uint32_t vchannel_id;
    time_t ts;
    uint8_t mode;									// из последнего
    int type_id;                    // typeID из MxReader и ZmqReceiver
    uint8_t subtype[20];							// из APPU или AUDP - имя протокола
    char app_proto[PROTONAME_MAX_LEN];			// из APPU или AUDP - имя протокола
    uint64_t segment_src;					// хэш для склейки сегментов, они берутся из APPU или AUDP
    uint64_t segment_dst;					// хэш для склейки сегментов, они берутся из APPU или AUDP
    int encap;										// решаем по последнему модулю
    uint8_t gre;									// флаг наличия gre в тракте. Вне разбора паспорта не используется
    uint16_t data_off;								// смещение данных от начала. Нормально 0
    GArray *desc;
    uint16_t data_len;								// длина (кол-во desc)
    uint8_t data[1];								// массив desc
} ctx_t;

typedef struct {
        short proto_id;
        const char *name;
        void *func;
} mx_table_t;

typedef struct {
    short proto_id;
    void(*init)();			// функция инициализации или NULL
    void(*func)(gpointer);	// функция, вызываемая перед обработкой каждого ключа. Через нее реализуется черный список ключей.
} proto_function_t;

#define _JSON	1
#define _SIG		2
#define _CER		4
#define _XML		8

#define HASHES_SIZE 0x1FF			// количество хешей
typedef struct {
    GSList *list_desc;				// список описателей: протоколов, адресов, портов
    uint64_t hash_src;				// хеш описателей для стороны источника
    uint64_t hash_dst;				// хеш описателей для стороны получателя
    int type_id;
    uint8_t out_set;				// обозначить, что идет на выход: json, sig или оба
    void *ctx;						// контекст
    uint32_t count;					// общее число уникальных порций данных
    uint32_t bad_count;				// число запоротых порций на обработке данных
    char *insert_before;			// строка вставки в начало вывода
    const char *insert_after;				// строка вставки в конец вывода
    char *xml_header;				// для xml вставка до и после тескста
    char *xml_footer;
    uint8_t func_mode;				// режим вызова func, чтобы разделять поведение в зависимости от места вызова
    void(*flush)(void *);			// функция сброса не собранных до конца данных
    void(*clean)(void *);			// функция очистки контекста, если он сложный и требует нечто большего, чем free()
    time_t t;						// время последнего доступа
    short proto_id;					// id протокола в массиве proto_name
    GString *text_out;				// вывод в json,xml
    GString *passport_text;			// паспорт для json или xml
    GByteArray *sig_out;			// вывод в sig
    GString *passport_sig;			// паспорт для sig
    GByteArray *cer_out;			// вывод в cer
    GString *passport_cer;			// паспорт для cer
    uint64_t hashes[HASHES_SIZE];	// циклический массив хешей вставляемых данных (для исключения дублей)
    int hash_current_element;		// текущая (последняя) позиция массиве хешей
    uint16_t cnt[2];
    void *_ctx;
} private_proto_data;

typedef struct {
    char proto_name[PROTONAME_MAX_LEN];
    uint64_t cnt[2];
} stat_table_item_t;

typedef struct {
    uint8_t src[32];
    uint8_t dst[32];
    uint8_t src_len;
    uint8_t dst_len;
    uint8_t proto[PROTONAME_MAX_LEN];
    uint16_t level;
    uint16_t index;
} desc;

typedef struct {
    uint8_t proto;
    void *data;
} proto;

typedef struct {
    uint8_t src[6];
    uint8_t dst[6];
    uint8_t type[2];
} ether_data;

typedef struct {
    uint8_t ip_src[16];
    uint8_t ip_dst[16];
    uint8_t ip_srclen;
    uint8_t ip_dstlen;
} ip_data;

typedef struct {
    unsigned int port_src;
    unsigned int port_dst;
} tcp_udp_data;

typedef struct {
    ctx_t *ctx;
    uint8_t out_set;
    int level;
    int(*func)(proto_node *, gpointer);
    GString *out;
    GSList  *src_list;
    epan_dissect_t *edt;
    proto_node *node;
    private_proto_data *pd;
    uint16_t packet_cnt;
    GString *packet;			// промежуточный буфер для сборки пакета
    const uint8_t *buf;
    uint16_t buf_len;			// обязательно ДВА байта!
} write_pdml_data;

void proto_log(const char *logname, const char * format, ...);
void init_mx_table();
void init_set();
const char *get_proto_name(short p);
gboolean meta_write_pdml_proto_tree(epan_dissect_t *edt, ctx_t *ctx, const guchar *buf, guint32 buf_len);
char *wrap_fvalue_to_string_repr(wmem_allocator_t *scope, fvalue_t *fv, ftrepr_t rtype, int field_display);
void wrap_wmem_free(wmem_allocator_t *allocator, void *ptr);
private_proto_data *new_proto_private_data(short proto_id, GSList *list_desc, uint64_t src, uint64_t dst, ctx_t *ctx);
private_proto_data *find_proto_private_data(short proto_id, uint64_t src, uint64_t dst, uint8_t *side);
int validate1(const char *buf, uint32_t len);
int make_hash(private_proto_data *pd, const char *buf, uint32_t len, uint8_t valid);
GSList* get_desc_list(write_pdml_data *pdata, uint64_t *src, uint64_t *dst);
void last_comma_delete(char *buf, uint32_t len);
uint32_t get_out_size(private_proto_data *pd);
int pd_validate(private_proto_data *pd);
void delete_proto_private_data(private_proto_data *pd);
void delete_proto_private_data_(private_proto_data *pd);
char *dot_replace(const char *str);
void set_spaces(GString *str, int count);
char* esc(char *str, uint32_t len);
GString *json_encode(char *str, int len);
int is_string(uint8_t *bytes, uint32_t size);
const guint8 *get_field_data(GSList *src_list, field_info *fi);
GString *stat_table_report();
void stat_table_add_cnt(uint32_t proto_id, uint32_t cnt_val);
void init_stat_table();

void arp_func(gpointer data);
void dns_func(gpointer data);
void cdp_func(gpointer data);
void nbns_func(gpointer data);
void bgp_func(gpointer data);
void dhcp_func(gpointer data);
void dcerpc_func(gpointer data);
void browser_func(gpointer data);
void diameter_func(gpointer data);
void gsm_a_rp_func(gpointer data);
void process_gsm_a_rp(proto_node *node, gpointer data);
void http_func(gpointer data);
void ftp_func(gpointer data);
void sip_func(gpointer data);
void imap_func(gpointer data);
void igrp_func(gpointer data);
void eigrp_json_func(gpointer data);
void ospf_func(gpointer data);
void ssl_func(gpointer data);
void snmp_func(gpointer data);
void smtp_func(gpointer data);
void stp_func(gpointer data);
void pop_func(gpointer data);
void ntp_func(gpointer data);
void goose_func(gpointer data);
void hsrp_func(gpointer data);
void isakmp_func(gpointer data);
void pap_func(gpointer data);
void rip_func(gpointer data);
void isis_func(gpointer data);
void telnet_func(gpointer data);
void rlogin_func(gpointer data);
void rsvp_func(gpointer data);
void rtsp_func(gpointer data);
void krb_func(gpointer data);
void mndp_func(gpointer data);
void ldp_func(gpointer data);
void lldp_func(gpointer data);
void ldap_func(gpointer data);
void gsm_sms_func(gpointer data);
void s7comm_func(gpointer data);
void syslog_func(gpointer data);
void social_func(gpointer data, const char *name, const char *proto, char *user);


#endif // META_OUT_H
