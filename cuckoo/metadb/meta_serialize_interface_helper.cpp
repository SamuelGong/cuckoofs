/* Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: MulanPSL-2.0
 */

extern "C" {
#include "metadb/meta_serialize_interface_helper.h"
}

#include "cuckoo_meta_param_generated.h"
#include "cuckoo_meta_response_generated.h"
#include "cuckoo_meta_rpc.pb.h"

static flatbuffers::FlatBufferBuilder FlatBufferBuilderPerProcess;

CuckooSupportMetaService MetaServiceTypeDecode(int32_t type)
{
    switch (type) {
    case cuckoo::meta_proto::MetaServiceType::MKDIR:
        return CuckooSupportMetaService::MKDIR;
    case cuckoo::meta_proto::MetaServiceType::MKDIR_SUB_MKDIR:
        return CuckooSupportMetaService::MKDIR_SUB_MKDIR;
    case cuckoo::meta_proto::MetaServiceType::MKDIR_SUB_CREATE:
        return CuckooSupportMetaService::MKDIR_SUB_CREATE;
    case cuckoo::meta_proto::MetaServiceType::CREATE:
        return CuckooSupportMetaService::CREATE;
    case cuckoo::meta_proto::MetaServiceType::STAT:
        return CuckooSupportMetaService::STAT;
    case cuckoo::meta_proto::MetaServiceType::OPEN:
        return CuckooSupportMetaService::OPEN;
    case cuckoo::meta_proto::MetaServiceType::CLOSE:
        return CuckooSupportMetaService::CLOSE;
    case cuckoo::meta_proto::MetaServiceType::UNLINK:
        return CuckooSupportMetaService::UNLINK;
    case cuckoo::meta_proto::MetaServiceType::READDIR:
        return CuckooSupportMetaService::READDIR;
    case cuckoo::meta_proto::MetaServiceType::OPENDIR:
        return CuckooSupportMetaService::OPENDIR;
    case cuckoo::meta_proto::MetaServiceType::RMDIR:
        return CuckooSupportMetaService::RMDIR;
    case cuckoo::meta_proto::MetaServiceType::RMDIR_SUB_RMDIR:
        return CuckooSupportMetaService::RMDIR_SUB_RMDIR;
    case cuckoo::meta_proto::MetaServiceType::RMDIR_SUB_UNLINK:
        return CuckooSupportMetaService::RMDIR_SUB_UNLINK;
    case cuckoo::meta_proto::MetaServiceType::RENAME:
        return CuckooSupportMetaService::RENAME;
    case cuckoo::meta_proto::MetaServiceType::RENAME_SUB_RENAME_LOCALLY:
        return CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY;
    case cuckoo::meta_proto::MetaServiceType::RENAME_SUB_CREATE:
        return CuckooSupportMetaService::RENAME_SUB_CREATE;
    case cuckoo::meta_proto::MetaServiceType::UTIMENS:
        return CuckooSupportMetaService::UTIMENS;
    case cuckoo::meta_proto::MetaServiceType::CHOWN:
        return CuckooSupportMetaService::CHOWN;
    case cuckoo::meta_proto::MetaServiceType::CHMOD:
        return CuckooSupportMetaService::CHMOD;
    default:
        return CuckooSupportMetaService::NOT_SUPPORTED;
    }
}

int32_t MetaServiceTypeEncode(CuckooSupportMetaService metaService)
{
    switch (metaService) {
    case CuckooSupportMetaService::MKDIR:
        return cuckoo::meta_proto::MetaServiceType::MKDIR;
    case CuckooSupportMetaService::MKDIR_SUB_MKDIR:
        return cuckoo::meta_proto::MetaServiceType::MKDIR_SUB_MKDIR;
    case CuckooSupportMetaService::MKDIR_SUB_CREATE:
        return cuckoo::meta_proto::MetaServiceType::MKDIR_SUB_CREATE;
    case CuckooSupportMetaService::CREATE:
        return cuckoo::meta_proto::MetaServiceType::CREATE;
    case CuckooSupportMetaService::STAT:
        return cuckoo::meta_proto::MetaServiceType::STAT;
    case CuckooSupportMetaService::OPEN:
        return cuckoo::meta_proto::MetaServiceType::OPEN;
    case CuckooSupportMetaService::CLOSE:
        return cuckoo::meta_proto::MetaServiceType::CLOSE;
    case CuckooSupportMetaService::UNLINK:
        return cuckoo::meta_proto::MetaServiceType::UNLINK;
    case CuckooSupportMetaService::READDIR:
        return cuckoo::meta_proto::MetaServiceType::READDIR;
    case CuckooSupportMetaService::OPENDIR:
        return cuckoo::meta_proto::MetaServiceType::OPENDIR;
    case CuckooSupportMetaService::RMDIR:
        return cuckoo::meta_proto::MetaServiceType::RMDIR;
    case CuckooSupportMetaService::RMDIR_SUB_RMDIR:
        return cuckoo::meta_proto::MetaServiceType::RMDIR_SUB_RMDIR;
    case CuckooSupportMetaService::RMDIR_SUB_UNLINK:
        return cuckoo::meta_proto::MetaServiceType::RMDIR_SUB_UNLINK;
    case CuckooSupportMetaService::RENAME:
        return cuckoo::meta_proto::MetaServiceType::RENAME;
    case CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY:
        return cuckoo::meta_proto::MetaServiceType::RENAME_SUB_RENAME_LOCALLY;
    case CuckooSupportMetaService::RENAME_SUB_CREATE:
        return cuckoo::meta_proto::MetaServiceType::RENAME_SUB_CREATE;
    case CuckooSupportMetaService::UTIMENS:
        return cuckoo::meta_proto::MetaServiceType::UTIMENS;
    case CuckooSupportMetaService::CHOWN:
        return cuckoo::meta_proto::MetaServiceType::CHOWN;
    case CuckooSupportMetaService::CHMOD:
        return cuckoo::meta_proto::MetaServiceType::CHMOD;
    default:
        return -1;
    }
}

bool SerializedDataMetaParamDecode(CuckooSupportMetaService metaService,
                                   int count,
                                   SerializedData *param,
                                   MetaProcessInfoData *infoArray)
{
    sd_size_t p = 0;
    for (int i = 0; i < count; ++i) {
        uint8_t *buffer = (uint8_t *)param->buffer + p;
        sd_size_t size = SerializedDataNextSeveralItemSize(param, p, 1);
        if (size == (sd_size_t)-1) {
            printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
            return false;
        }

        uint8_t *itemBuffer = (uint8_t *)buffer + SERIALIZED_DATA_ALIGNMENT;
        size_t itemSize = size - SERIALIZED_DATA_ALIGNMENT;
        flatbuffers::Verifier verifier(itemBuffer, itemSize);
        if (!verifier.VerifyBuffer<cuckoo::meta_fbs::MetaParam>(NULL)) {
            printf("[debug] itemSize = %lu, serialized param is corrupt: %s:%d\n", itemSize, __FILE__, __LINE__);
            return false;
        }
        auto metaParam = cuckoo::meta_fbs::GetMetaParam(itemBuffer);

        MetaProcessInfo info = infoArray + i;
        switch (metaService) {
        case CuckooSupportMetaService::MKDIR:
        case CuckooSupportMetaService::CREATE:
        case CuckooSupportMetaService::STAT:
        case CuckooSupportMetaService::OPEN:
        case CuckooSupportMetaService::UNLINK:
        case CuckooSupportMetaService::OPENDIR:
        case CuckooSupportMetaService::RMDIR: {
            // path only param
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_PathOnlyParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            info->path = metaParam->param_as_PathOnlyParam()->path()->c_str();
            break;
        }
        case CuckooSupportMetaService::MKDIR_SUB_MKDIR: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_MkdirSubMkdirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto mkdirSubMkdirParam = metaParam->param_as_MkdirSubMkdirParam();
            info->parentId = mkdirSubMkdirParam->parent_id();
            info->name = const_cast<char *>(mkdirSubMkdirParam->name()->c_str());
            info->inodeId = mkdirSubMkdirParam->inode_id();
            break;
        }
        case CuckooSupportMetaService::MKDIR_SUB_CREATE: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_MkdirSubCreateParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto mkdirSubCreateParam = metaParam->param_as_MkdirSubCreateParam();
            info->parentId_partId = mkdirSubCreateParam->parent_id_part_id();
            info->name = const_cast<char *>(mkdirSubCreateParam->name()->c_str());
            info->inodeId = mkdirSubCreateParam->inode_id();
            info->st_mode = mkdirSubCreateParam->st_mode();
            info->st_size = mkdirSubCreateParam->st_size();
            info->st_mtim = mkdirSubCreateParam->st_mtim();
            break;
        }
        case CuckooSupportMetaService::CLOSE: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_CloseParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto closeParam = metaParam->param_as_CloseParam();
            info->path = closeParam->path()->c_str();
            info->st_size = closeParam->st_size();
            info->st_mtim = closeParam->st_mtim();
            info->node_id = closeParam->node_id();
            break;
        }
        case CuckooSupportMetaService::READDIR: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_ReadDirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto readDirParam = metaParam->param_as_ReadDirParam();
            info->path = readDirParam->path()->c_str();
            info->readDirMaxReadCount = readDirParam->max_read_count();
            info->readDirLastShardIndex = readDirParam->last_shard_index();
            info->readDirLastFileName = readDirParam->last_file_name()->c_str();
            break;
        }
        case CuckooSupportMetaService::RMDIR_SUB_RMDIR: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_RmdirSubRmdirParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto rmdirSubRmdirParam = metaParam->param_as_RmdirSubRmdirParam();
            info->parentId = rmdirSubRmdirParam->parent_id();
            info->name = const_cast<char *>(rmdirSubRmdirParam->name()->c_str());
            break;
        }
        case CuckooSupportMetaService::RMDIR_SUB_UNLINK: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_RmdirSubUnlinkParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto rmdirSubUnlinkParam = metaParam->param_as_RmdirSubUnlinkParam();
            info->parentId_partId = rmdirSubUnlinkParam->parent_id_part_id();
            info->name = const_cast<char *>(rmdirSubUnlinkParam->name()->c_str());
            break;
        }
        case CuckooSupportMetaService::RENAME: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_RenameParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameParam = metaParam->param_as_RenameParam();
            info->path = renameParam->src()->c_str();
            info->dstPath = renameParam->dst()->c_str();
            break;
        }
        case CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_RenameSubRenameLocallyParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameSubRenameLocallyParam = metaParam->param_as_RenameSubRenameLocallyParam();
            info->parentId = renameSubRenameLocallyParam->src_parent_id();
            info->parentId_partId = renameSubRenameLocallyParam->src_parent_id_part_id();
            info->name = const_cast<char *>(renameSubRenameLocallyParam->src_name()->c_str());
            info->dstParentId =
                renameSubRenameLocallyParam->dst_parent_id(); // TOFO: ymz: modify 'dstParentIdPartId' to 'dstParentId'
            info->dstParentIdPartId = renameSubRenameLocallyParam->dst_parent_id_part_id();
            info->dstName = const_cast<char *>(renameSubRenameLocallyParam->dst_name()->c_str());
            info->targetIsDirectory = renameSubRenameLocallyParam->target_is_directory();
            info->srcLockOrder = renameSubRenameLocallyParam->src_lock_order();
            info->inodeId = renameSubRenameLocallyParam->directory_inode_id();
            break;
        }
        case CuckooSupportMetaService::RENAME_SUB_CREATE: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_RenameSubCreateParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto renameSubCreateParam = metaParam->param_as_RenameSubCreateParam();
            info->parentId_partId = renameSubCreateParam->parentid_partid();
            info->name = const_cast<char *>(renameSubCreateParam->name()->c_str());
            info->inodeId = renameSubCreateParam->st_ino();
            info->st_dev = renameSubCreateParam->st_dev();
            info->st_mode = renameSubCreateParam->st_mode();
            info->st_nlink = renameSubCreateParam->st_nlink();
            info->st_uid = renameSubCreateParam->st_uid();
            info->st_gid = renameSubCreateParam->st_gid();
            info->st_rdev = renameSubCreateParam->st_rdev();
            info->st_size = renameSubCreateParam->st_size();
            info->st_blksize = renameSubCreateParam->st_blksize();
            info->st_blocks = renameSubCreateParam->st_blocks();
            info->st_atim = renameSubCreateParam->st_atim();
            info->st_mtim = renameSubCreateParam->st_mtim();
            info->st_ctim = renameSubCreateParam->st_ctim();
            info->node_id = renameSubCreateParam->node_id();
            break;
        }
        case CuckooSupportMetaService::UTIMENS: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_UtimeNsParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto utimeNsParam = metaParam->param_as_UtimeNsParam();
            info->path = utimeNsParam->path()->c_str();
            info->st_atim = utimeNsParam->st_atim();
            info->st_mtim = utimeNsParam->st_mtim();
            break;
        }
        case CuckooSupportMetaService::CHOWN: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_ChownParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto chownParam = metaParam->param_as_ChownParam();
            info->path = chownParam->path()->c_str();
            info->st_uid = chownParam->st_uid();
            info->st_gid = chownParam->st_gid();
            break;
        }
        case CuckooSupportMetaService::CHMOD: {
            if (metaParam->param_type() != cuckoo::meta_fbs::AnyMetaParam::AnyMetaParam_ChmodParam) {
                printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
                return false;
            }
            auto chmodParam = metaParam->param_as_ChmodParam();
            info->path = chmodParam->path()->c_str();
            info->st_mode = chmodParam->st_mode();
            break;
        }
        default:
            printf("[debug] serialized param is corrupt: %s:%d\n", __FILE__, __LINE__);
            return false;
        }

        p += size;
    }
    return true;
}

bool SerializedDataMetaParamEncode(CuckooSupportMetaService metaService,
                                   MetaProcessInfo *infoArray,
                                   int32_t *index,
                                   int count,
                                   flatbuffers::FlatBufferBuilder &builder,
                                   SerializedData *param)
{
    for (int i = 0; i < count; ++i) {
        MetaProcessInfo info = index == NULL ? (infoArray[i]) : (infoArray[index[i]]);

        builder.Clear();
        flatbuffers::Offset<cuckoo::meta_fbs::MetaParam> metaParam;
        switch (metaService) {
        case CuckooSupportMetaService::MKDIR_SUB_MKDIR: {
            auto mkdirSubMkdirParam =
                cuckoo::meta_fbs::CreateMkdirSubMkdirParamDirect(builder, info->parentId, info->name, info->inodeId);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_MkdirSubMkdirParam,
                                                          mkdirSubMkdirParam.Union());
            break;
        }
        case CuckooSupportMetaService::MKDIR_SUB_CREATE: {
            auto mkdirSubCreateParam = cuckoo::meta_fbs::CreateMkdirSubCreateParamDirect(builder,
                                                                                         info->parentId_partId,
                                                                                         info->name,
                                                                                         info->inodeId,
                                                                                         info->st_mode,
                                                                                         info->st_mtim,
                                                                                         info->st_size);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_MkdirSubCreateParam,
                                                          mkdirSubCreateParam.Union());
            break;
        }
        case CuckooSupportMetaService::RMDIR_SUB_RMDIR: {
            auto rmdirSubRmdirParam =
                cuckoo::meta_fbs::CreateRmdirSubRmdirParamDirect(builder, info->parentId, info->name);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_RmdirSubRmdirParam,
                                                          rmdirSubRmdirParam.Union());
            break;
        }
        case CuckooSupportMetaService::RMDIR_SUB_UNLINK: {
            auto rmdirSubUnlinkParam =
                cuckoo::meta_fbs::CreateRmdirSubUnlinkParamDirect(builder, info->parentId_partId, info->name);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_RmdirSubUnlinkParam,
                                                          rmdirSubUnlinkParam.Union());
            break;
        }
        case CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            auto renameSubRenameLocallyParam =
                cuckoo::meta_fbs::CreateRenameSubRenameLocallyParamDirect(builder,
                                                                          info->parentId,
                                                                          info->parentId_partId,
                                                                          info->name,
                                                                          info->dstParentId,
                                                                          info->dstParentIdPartId,
                                                                          info->dstName,
                                                                          info->targetIsDirectory,
                                                                          info->inodeId,
                                                                          info->srcLockOrder);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_RenameSubRenameLocallyParam,
                                                          renameSubRenameLocallyParam.Union());
            break;
        }
        case CuckooSupportMetaService::RENAME_SUB_CREATE: {
            auto renameSubCreateParam = cuckoo::meta_fbs::CreateRenameSubCreateParamDirect(builder,
                                                                                           info->parentId_partId,
                                                                                           info->name,
                                                                                           info->inodeId,
                                                                                           info->st_dev,
                                                                                           info->st_mode,
                                                                                           info->st_nlink,
                                                                                           info->st_uid,
                                                                                           info->st_gid,
                                                                                           info->st_rdev,
                                                                                           info->st_size,
                                                                                           info->st_blksize,
                                                                                           info->st_blocks,
                                                                                           info->st_atim,
                                                                                           info->st_mtim,
                                                                                           info->st_ctim,
                                                                                           info->node_id);
            metaParam = cuckoo::meta_fbs::CreateMetaParam(builder,
                                                          cuckoo::meta_fbs::AnyMetaParam_RenameSubCreateParam,
                                                          renameSubCreateParam.Union());
            break;
        }
        default:
            return false;
        }
        builder.Finish(metaParam);

        char *buffer = SerializedDataApplyForSegment(param, builder.GetSize());
        memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
    }
    return true;
}

bool SerializedDataMetaParamEncodeWithPerProcessFlatBufferBuilder(CuckooSupportMetaService metaService,
                                                                  MetaProcessInfo *infoArray,
                                                                  int32_t *index,
                                                                  int count,
                                                                  SerializedData *param)
{
    return SerializedDataMetaParamEncode(metaService, infoArray, index, count, FlatBufferBuilderPerProcess, param);
}

bool SerializedDataMetaResponseDecode(CuckooSupportMetaService metaService,
                                      int count,
                                      SerializedData *response,
                                      MetaProcessInfoData *infoArray)
{
    sd_size_t p = 0;
    for (int i = 0; i < count; i++) {
        uint8_t *buffer = (uint8_t *)response->buffer + p;
        sd_size_t size = SerializedDataNextSeveralItemSize(response, p, 1);
        if (size == (sd_size_t)-1)
            return false;

        uint8_t *itemBuffer = (uint8_t *)buffer + SERIALIZED_DATA_ALIGNMENT;
        size_t itemSize = size - SERIALIZED_DATA_ALIGNMENT;
        flatbuffers::Verifier verifier(itemBuffer, itemSize);
        if (!verifier.VerifyBuffer<cuckoo::meta_fbs::MetaResponse>(NULL))
            return false;
        auto metaResponse = cuckoo::meta_fbs::GetMetaResponse(itemBuffer);

        MetaProcessInfo info = infoArray + i;
        info->errorCode = (CuckooErrorCode)metaResponse->error_code();
        switch (metaService) {
        case CuckooSupportMetaService::MKDIR_SUB_MKDIR:
        case CuckooSupportMetaService::MKDIR_SUB_CREATE:
        case CuckooSupportMetaService::RMDIR_SUB_RMDIR:
        case CuckooSupportMetaService::RMDIR_SUB_UNLINK:
        case CuckooSupportMetaService::RENAME_SUB_CREATE: {
            // Error code only. Do nothing.
            break;
        }
        case CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
            if (metaResponse->response_type() == cuckoo::meta_fbs::AnyMetaResponse::AnyMetaResponse_NONE) {
                // No extra data returned. Do nothing.
            } else if (metaResponse->response_type() ==
                       cuckoo::meta_fbs::AnyMetaResponse::AnyMetaResponse_RenameSubRenameLocallyResponse) {
                //
                auto renameSubRenameLocallyResponse = metaResponse->response_as_RenameSubRenameLocallyResponse();
                info->inodeId = renameSubRenameLocallyResponse->st_ino();
                info->st_dev = renameSubRenameLocallyResponse->st_dev();
                info->st_mode = renameSubRenameLocallyResponse->st_mode();
                info->st_nlink = renameSubRenameLocallyResponse->st_nlink();
                info->st_uid = renameSubRenameLocallyResponse->st_uid();
                info->st_gid = renameSubRenameLocallyResponse->st_gid();
                info->st_rdev = renameSubRenameLocallyResponse->st_rdev();
                info->st_size = renameSubRenameLocallyResponse->st_size();
                info->st_blksize = renameSubRenameLocallyResponse->st_blksize();
                info->st_blocks = renameSubRenameLocallyResponse->st_blocks();
                info->st_atim = renameSubRenameLocallyResponse->st_atim();
                info->st_mtim = renameSubRenameLocallyResponse->st_mtim();
                info->st_ctim = renameSubRenameLocallyResponse->st_ctim();
                info->node_id = renameSubRenameLocallyResponse->node_id();
            } else
                return false;
            break;
        }
        default:
            return false;
        }

        p += size;
    }
    return true;
}

static bool SerializedDataMetaResponseEncode(CuckooSupportMetaService metaService,
                                             int count,
                                             MetaProcessInfoData *infoArray,
                                             flatbuffers::FlatBufferBuilder &builder,
                                             SerializedData *response)
{
    for (int i = 0; i < count; ++i) {
        builder.Clear();
        MetaProcessInfo info = infoArray + i;
        flatbuffers::Offset<cuckoo::meta_fbs::MetaResponse> metaResponse;
        if (info->errorCode != SUCCESS && info->errorCode != FILE_EXISTS) {
            //
            metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder, info->errorCode);
        } else {
            switch (metaService) {
            case CuckooSupportMetaService::MKDIR:
            case CuckooSupportMetaService::MKDIR_SUB_MKDIR:
            case CuckooSupportMetaService::MKDIR_SUB_CREATE:
            case CuckooSupportMetaService::CLOSE:
            case CuckooSupportMetaService::RMDIR:
            case CuckooSupportMetaService::RMDIR_SUB_RMDIR:
            case CuckooSupportMetaService::RMDIR_SUB_UNLINK:
            case CuckooSupportMetaService::RENAME:
            case CuckooSupportMetaService::RENAME_SUB_CREATE:
            case CuckooSupportMetaService::UTIMENS:
            case CuckooSupportMetaService::CHOWN:
            case CuckooSupportMetaService::CHMOD: {
                // error code only response
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder, info->errorCode);
                break;
            }
            case CuckooSupportMetaService::CREATE: {
                // auto createResponse = cuckoo::meta_fbs::CreateCreateResponse(builder, info->inodeId);
                auto createResponse = cuckoo::meta_fbs::CreateCreateResponse(builder,
                                                                             info->inodeId,
                                                                             info->node_id,
                                                                             info->st_dev,
                                                                             info->st_mode,
                                                                             info->st_nlink,
                                                                             info->st_uid,
                                                                             info->st_gid,
                                                                             info->st_rdev,
                                                                             info->st_size,
                                                                             info->st_blksize,
                                                                             info->st_blocks,
                                                                             info->st_atim,
                                                                             info->st_mtim,
                                                                             info->st_ctim);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_CreateResponse,
                                                                    createResponse.Union());
                break;
            }
            case CuckooSupportMetaService::STAT: {
                auto statResponse = cuckoo::meta_fbs::CreateStatResponse(builder,
                                                                         info->inodeId,
                                                                         info->st_dev,
                                                                         info->st_mode,
                                                                         info->st_nlink,
                                                                         info->st_uid,
                                                                         info->st_gid,
                                                                         info->st_rdev,
                                                                         info->st_size,
                                                                         info->st_blksize,
                                                                         info->st_blocks,
                                                                         info->st_atim,
                                                                         info->st_mtim,
                                                                         info->st_ctim);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_StatResponse,
                                                                    statResponse.Union());
                break;
            }
            case CuckooSupportMetaService::OPEN: {
                auto openResponse = cuckoo::meta_fbs::CreateOpenResponse(builder,
                                                                         info->inodeId,
                                                                         info->node_id,
                                                                         info->st_dev,
                                                                         info->st_mode,
                                                                         info->st_nlink,
                                                                         info->st_uid,
                                                                         info->st_gid,
                                                                         info->st_rdev,
                                                                         info->st_size,
                                                                         info->st_blksize,
                                                                         info->st_blocks,
                                                                         info->st_atim,
                                                                         info->st_mtim,
                                                                         info->st_ctim);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_OpenResponse,
                                                                    openResponse.Union());
                break;
            }
            case CuckooSupportMetaService::UNLINK: {
                auto unlinkResponse =
                    cuckoo::meta_fbs::CreateUnlinkResponse(builder, info->inodeId, info->st_size, info->node_id);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_UnlinkResponse,
                                                                    unlinkResponse.Union());
                break;
            }
            case CuckooSupportMetaService::READDIR: {
                std::vector<flatbuffers::Offset<cuckoo::meta_fbs::OneReadDirResponse>> readDirResultList;
                for (int j = 0; j < info->readDirResultCount; ++j)
                    readDirResultList.push_back(
                        cuckoo::meta_fbs::CreateOneReadDirResponseDirect(builder,
                                                                         info->readDirResultList[j]->fileName,
                                                                         info->readDirResultList[j]->mode));
                auto readDirResponse = cuckoo::meta_fbs::CreateReadDirResponseDirect(builder,
                                                                                     info->readDirLastShardIndex,
                                                                                     info->readDirLastFileName,
                                                                                     &readDirResultList);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_ReadDirResponse,
                                                                    readDirResponse.Union());
                break;
            }
            case CuckooSupportMetaService::OPENDIR: {
                auto openDirResponse = cuckoo::meta_fbs::CreateOpenDirResponse(builder, info->inodeId);
                metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder,
                                                                    info->errorCode,
                                                                    cuckoo::meta_fbs::AnyMetaResponse_OpenDirResponse,
                                                                    openDirResponse.Union());
                break;
            }
            case CuckooSupportMetaService::RENAME_SUB_RENAME_LOCALLY: {
                if (info->parentId_partId != 0 && info->dstParentIdPartId == 0) {
                    auto renameSubRenameLocallyResponse =
                        cuckoo::meta_fbs::CreateRenameSubRenameLocallyResponse(builder,
                                                                               info->inodeId,
                                                                               info->st_dev,
                                                                               info->st_mode,
                                                                               info->st_nlink,
                                                                               info->st_uid,
                                                                               info->st_gid,
                                                                               info->st_rdev,
                                                                               info->st_size,
                                                                               info->st_blksize,
                                                                               info->st_blocks,
                                                                               info->st_atim,
                                                                               info->st_mtim,
                                                                               info->st_ctim,
                                                                               info->node_id);
                    metaResponse = cuckoo::meta_fbs::CreateMetaResponse(
                        builder,
                        info->errorCode,
                        cuckoo::meta_fbs::AnyMetaResponse_RenameSubRenameLocallyResponse,
                        renameSubRenameLocallyResponse.Union());
                } else {
                    metaResponse = cuckoo::meta_fbs::CreateMetaResponse(builder, info->errorCode);
                }
                break;
            }
            default:
                return false;
            }
        }
        builder.Finish(metaResponse);

        char *buffer = SerializedDataApplyForSegment(response, builder.GetSize());
        memcpy(buffer, builder.GetBufferPointer(), builder.GetSize());
    }
    return true;
}

bool SerializedDataMetaResponseEncodeWithPerProcessFlatBufferBuilder(CuckooSupportMetaService metaService,
                                                                     int count,
                                                                     MetaProcessInfoData *infoArray,
                                                                     SerializedData *response)
{
    return SerializedDataMetaResponseEncode(metaService, count, infoArray, FlatBufferBuilderPerProcess, response);
}