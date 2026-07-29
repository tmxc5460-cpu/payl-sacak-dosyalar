/*
 * TMXC_OS - TMXC OS İşletim Sistemi
 * Copyright (c) 2024 TMXC_OS Development Team
 * Tüm hakları saklıdır.
 * 
 * Bu dosya TMXC_OS projesinin bir parçasıdır ve lisans altında korunmaktadır.
 * İzinsiz kopyalanması, dağıtılması veya değiştirilmesi yasaktır.
 * 
 * Lisans Bilgileri:
 * - Lisans Türü: PROPRIETARY
 * - Sahip: TMXC OS / TMXC_OS Team
 * - Kullanım Koşulları: Sadece lisans sahibi tarafından kullanılabilir
 * 
 * İletişim: license@tmxc-os.com
 * Web: www.tmxc-os.com
 * 
 * Yasal Uyarı:
 * Bu yazılımın herhangi bir kısmının izinsiz kullanımı,
 * kopyalanması, dağıtılması veya ticari amaçla kullanılması
 * Türk Ceza Kanunu ve Uluslararası Telif Hakkı yasaları
 * kapsamında suç teşkil eder.
 * 
 * Lisans Doğrulama:
 * Bu yazılım lisans doğrulama sistemi içerir.
 * Lisans anahtarı olmadan çalışmaz.
 */

/*
.
#ifndef TMXC_OFFLINE_GOOGLE_BRIDGE_H
#define TMXC_OFFLINE_GOOGLE_BRIDGE_H

#include "../tmxc_kernel.h"

#define TMXC_OFFLINE_KNOWLEDGE_DB_SIZE 10000
#define TMXC_OFFLINE_SEARCH_RESULTS 10
#define TMXC_OFFLINE_MAX_QUERY_LENGTH 256
#define TMXC_OFFLINE_CACHE_SIZE 500

typedef struct {
    char keyword[64];
    char content[512];
    uint32_t relevance_score;
    uint32_t access_count;
    uint64_t last_accessed;
    uint8_t active;
} tmxc_knowledge_entry_t;

typedef struct {
    char query[TMXC_OFFLINE_MAX_QUERY_LENGTH];
    char results[TMXC_OFFLINE_SEARCH_RESULTS][512];
    uint32_t result_count;
    uint32_t total_matches;
    uint64_t query_time;
    uint8_t from_cache;
} tmxc_search_result_t;

typedef struct {
    tmxc_knowledge_entry_t knowledge_db[TMXC_OFFLINE_KNOWLEDGE_DB_SIZE];
    uint32_t db_entry_count;
    
    tmxc_search_result_t cache[TMXC_OFFLINE_CACHE_SIZE];
    uint32_t cache_index;
    
    uint8_t offline_mode;
    uint8_t initialized;
    uint64_t total_queries;
    uint64_t cache_hits;
} tmxc_offline_google_bridge_t;

void tmxc_offline_google_bridge_init(void);
void tmxc_offline_google_bridge_enable_offline_mode(uint8_t enable);
uint8_t tmxc_offline_google_bridge_is_offline_mode_enabled(void);

uint32_t tmxc_offline_google_bridge_add_knowledge(const char* keyword, const char* content, uint32_t relevance);
void tmxc_offline_google_bridge_remove_knowledge(const char* keyword);

tmxc_search_result_t* tmxc_offline_google_bridge_search(const char* query);
tmxc_search_result_t* tmxc_offline_google_bridge_search_cached(const char* query);

void tmxc_offline_google_bridge_clear_cache(void);
void tmxc_offline_google_bridge_preload_common_knowledge(void);

uint64_t tmxc_offline_google_bridge_get_total_queries(void);
uint64_t tmxc_offline_google_bridge_get_cache_hits(void);
float tmxc_offline_google_bridge_get_cache_hit_rate(void);

void tmxc_offline_google_bridge_cleanup(void);

#endif
