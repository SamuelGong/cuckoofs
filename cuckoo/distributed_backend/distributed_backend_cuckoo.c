/* Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: MulanPSL-2.0
 */

#include "distributed_backend/distributed_backend_cuckoo.h"

#include "executor/spi.h"

#include "metadb/inode_table.h"
#include "metadb/shard_table.h"
#include "metadb/xattr_table.h"
#include "utils/error_log.h"
#include "utils/utils.h"

PG_FUNCTION_INFO_V1(cuckoo_create_distributed_data_table);
PG_FUNCTION_INFO_V1(cuckoo_prepare_commands);

Datum cuckoo_create_distributed_data_table(PG_FUNCTION_ARGS)
{
    CuckooCreateDistributedDataTable();

    PG_RETURN_INT16(SUCCESS);
}

Datum cuckoo_prepare_commands(PG_FUNCTION_ARGS)
{
    CuckooPrepareCommands();

    PG_RETURN_INT16(SUCCESS);
}

void CuckooCreateDistributedDataTable()
{
    List *shardTableData = GetShardTableData();

    StringInfo toExecCommand = makeStringInfo();
    StringInfo name = makeStringInfo();
    for (int i = 0; i < list_length(shardTableData); ++i) {
        Form_cuckoo_shard_table data = list_nth(shardTableData, i);
        if (data->server_id != GetLocalServerId())
            continue;

        resetStringInfo(name);
        appendStringInfo(name, "%s_%d", InodeTableName, data->range_point);
        if (CheckIfRelationExists(name->data, PG_CATALOG_NAMESPACE)) //
            continue;
        ConstructCreateInodeTableCommand(toExecCommand, name->data);

        resetStringInfo(name);
        appendStringInfo(name, "%s_%d", XattrTableName, data->range_point);
        ConstructCreateXattrTableCommand(toExecCommand, name->data);
    }
    if (toExecCommand->len == 0)
        return;

    int spiConnectionResult = SPI_connect();
    if (spiConnectionResult != SPI_OK_CONNECT) {
        SPI_finish();
        CUCKOO_ELOG_ERROR(PROGRAM_ERROR, "could not connect to SPI manager.");
    }

    int spiQueryResult = SPI_execute(toExecCommand->data, false, 0);
    if (spiQueryResult != SPI_OK_UTILITY) {
        SPI_finish();
        CUCKOO_ELOG_ERROR(PROGRAM_ERROR, "spi exec failed.");
    }
    SPI_finish();
}

void CuckooPrepareCommands()
{
    static bool Prepared = false;
    if (Prepared)
        return;
    int spiConnectionResult = SPI_connect();
    if (spiConnectionResult != SPI_OK_CONNECT) {
        SPI_finish();
        CUCKOO_ELOG_ERROR(PROGRAM_ERROR, "could not connect to SPI manager.");
    }

    const char *commands[] = {
        "PREPARE cs_meta_call(int, int, bytea) AS SELECT cuckoo_meta_call_by_serialized_data($1, $2, $3);",
    };

    for (int i = 0; i < sizeof(commands) / sizeof(char *); ++i) {
        int spiQueryResult = SPI_execute(commands[i], false, 0);
        if (spiQueryResult != SPI_OK_UTILITY) {
            SPI_finish();
            CUCKOO_ELOG_ERROR(PROGRAM_ERROR, "spi exec failed.");
        }
    }
    SPI_finish();

    Prepared = true;
}