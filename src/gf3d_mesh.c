#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf3d_obj_load.h"
#include "gf3d_buffers.h"

#include "gf3d_mesh.h"
#define MESH_ATTRIBUTE_COUNT 3

typedef struct {
    Uint32 mesh_count;
    Mesh* mesh_list;
    VkDevice device;
    VkVertexInputAttributeDescription attributeDescriptions[MESH_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription bindingDescription;

}MeshManager;

static MeshManager mesh_manager = { 0 };

//////forward declare
void gf3d_mesh_primitive_free(MeshPrimitive* prim);
void gf3d_mesh_delete(Mesh* mesh);

//////done doing that


void gf3d_mesh_init(Uint32 mesh_max) {
    if (!mesh_max)
    {
        slog("need mesh count to be nonzero");
        return;
    }
    if (mesh_manager.mesh_count != 0)
    {
        slog("mesh manager already exists");
        return;
    }

    //include attribute bindings here please

    mesh_manager.mesh_list = (Mesh*)gfc_allocate_array(sizeof(Mesh), mesh_max);
    mesh_manager.mesh_count = mesh_max;

    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

}


Mesh* gf3d_mesh_new()
{
    for (int i = 0; i < mesh_manager.mesh_count; i++)
    {
        if ((mesh_manager.mesh_list[i]._refCount == 0)) {
            gf3d_mesh_free(&mesh_manager.mesh_list[i]);
            mesh_manager.mesh_list[i]._refCount = 1;
            return &mesh_manager.mesh_list[i];
        }
        if (!mesh_manager.mesh_list[i]._refCount)
        {
            gf3d_mesh_free(&mesh_manager.mesh_list[i]);
            mesh_manager.mesh_list[i]._refCount = 1;
            return &mesh_manager.mesh_list[i];
        }
    }
    slog("no open mesh space");
    return NULL;
}

Mesh* gf3d_mesh_get_by_filename(const char* filename)
{
    if (!filename)return NULL;
    for (int i = 0; i < mesh_manager.mesh_count; i++)
    {
        if ((mesh_manager.mesh_list[i]._refCount == 0) ||
            (mesh_manager.mesh_list[i].filename == NULL))
        {
            continue;
        }
        if (gfc_strlcmp(filename, mesh_manager.mesh_list[i].filename))
        {
            return &mesh_manager.mesh_list[i];
        }
    }
    return NULL;
}

Mesh* gf3d_mesh_load_obj(const char* filename) {
    const char* str = NULL;
    Mesh* mesh;
    MeshPrimitive* prim;
    ObjData* objData;


    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh)
    {
        //mesh already loaded
        mesh->_refCount++;
        return mesh;
    }
    mesh = gf3d_mesh_new();
    if (!mesh)
    {
        slog("no mesh");
        return NULL;
    }

    prim = gf3d_mesh_primitive_new();

    if (!prim)
    {
        slog("failed to load mesh primitive");
        return NULL;
    }


    objData = gf3d_obj_load_from_file(filename);
    if (!objData)
    {
        gf3d_mesh_delete(mesh);
        gf3d_mesh_primitive_free(prim);
        slog("failed to allocate objData for mesh");
        return NULL;
    }

    slog("data prim created");

    prim->objData = objData;

    if (!gf3d_prim_buffer_create(prim))
    {
        gf3d_mesh_delete(mesh);
        gf3d_mesh_primitive_free(prim);
        slog("failed to create prim buffers");
        return NULL;
    }

    gfc_list_append(mesh->primitives, prim);
    gfc_line_cpy(mesh->filename, filename);

    slog("successfully loaded obj from file %s", filename);

    return mesh;
    
}


Mesh* gf3d_mesh_copy(Mesh* in)
{
    //i think this should work?
    Mesh* mesh = gf3d_mesh_new();
    mesh->bounds.d = in->bounds.d;
    mesh->bounds.h = in->bounds.h;
    mesh->bounds.w = in->bounds.w;
    mesh->bounds.x = in->bounds.x;
    mesh->bounds.y = in->bounds.y;
    mesh->bounds.z = in->bounds.z;

    strcpy(mesh->filename, in->filename);
    mesh->primitives = gfc_list_copy(in->filename);
    return mesh;
}

void gf3d_mesh_move_vertices(Mesh* in, GFC_Vector3D offset, GFC_Vector3D rotation)
{
    //figure this out later??
}


MeshPrimitive* gf3d_mesh_primitive_new() {
    MeshPrimitive* primitive;
    primitive = gfc_allocate_array(sizeof(MeshPrimitive), 1);


    if (!primitive)return NULL;

    return primitive; 

}

void gf3d_mesh_primitive_free(MeshPrimitive* prim)
{
    if (!prim) {
        return;
    }

    if (prim->faceBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(mesh_manager.device, prim->faceBuffer, NULL);
        prim->faceBuffer = VK_NULL_HANDLE;
    }
    if (prim->faceBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(mesh_manager.device, prim->faceBufferMemory, NULL);
        prim->faceBufferMemory = VK_NULL_HANDLE;
    }


    if (prim->vertexBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(mesh_manager.device, prim->vertexBuffer, NULL);
        prim->vertexBuffer = VK_NULL_HANDLE;
    }
    if (prim->vertexBufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(mesh_manager.device, prim->vertexBufferMemory, NULL);
        prim->vertexBufferMemory = VK_NULL_HANDLE;
    }


    if (prim->objData)
    {
        gf3d_obj_free(prim-> objData);
    }

    memset(prim, 0, sizeof(MeshPrimitive));

}


Uint8 gf3d_primitive_buffer_create(MeshPrimitive* prim) {
    Uint32 bufferSize = 0;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDevice device = mesh_manager.device;
    void* data = NULL;

    if (!prim)
    {
        slog("no prim to create buffer");
        return 0;
    }
    if (!prim->objData)
    {
        slog("no prim objData");
        return 0;
    }
    if (!prim->objData->face_count)
    {
        slog("no prim objData face_count");
        return 0;
    }

    //face buffers
    bufferSize = (sizeof(Face) * prim->objData->face_count);
    gf3d_buffer_create(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        &stagingBuffer, 
        &stagingBufferMemory
    );
    vkMapMemory(
        device,
        stagingBufferMemory,
        0,
        bufferSize,
        0,
        &data
    ) != VK_SUCCESS;
    memcpy(data, prim->objData->outFace, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &prim->faceBuffer,
        &prim->faceBufferMemory
    );

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);



    //vertex buffers
    bufferSize = (sizeof(Vertex) * prim->objData->vertex_count);
    gf3d_buffer_create(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer,
        &stagingBufferMemory
    );
    vkMapMemory(
        device,
        stagingBufferMemory,
        0,
        bufferSize,
        0,
        &data
    );
    memcpy(data, prim->objData->faceVertices, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &prim->vertexBuffer,
        &prim->vertexBufferMemory
    );

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    prim->faceCount = prim->objData->face_count;
    prim->vertexCount = prim->objData->face_vert_count;

    return 1;


}

/**
 * @brief get the input attribute descriptions for mesh based rendering
 * @param count (optional, output) the number of attributes
 * @return a pointer to a vertex input attribute description array
 */
VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(Uint32* count)
{
    return mesh_manager.attributeDescriptions;
}

/**
 * @brief get the binding description for mesh based rendering
 * @return vertex input binding descriptions compatible with mesh data
 */
VkVertexInputBindingDescription* gf3d_mesh_get_bind_description()
{
    return &mesh_manager.bindingDescription;
}

void gf3d_mesh_delete(Mesh* mesh) {
    if (!mesh)return;
    int count = 0;
    MeshPrimitive* prim;
    count = gfc_list_count(mesh->primitives);
    for (int i = 0; i < count; i++)
    {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;
        gf3d_mesh_primitive_free(prim);
    }

    if (mesh->primitives)
    {
        gfc_list_delete(mesh->primitives);
        mesh->primitives = NULL;
    }

    memset(mesh, 0, sizeof(Mesh));
}

/**
 * @brief free a mesh that has been loaded from memory
 */
void gf3d_mesh_free(Mesh* mesh) {
    mesh->_refCount--;
    if (mesh->_refCount == 0)
    {
        gf3d_mesh_delete(mesh);
    }
}



void gf3d_mesh_queue_render(Mesh* mesh, Pipeline* pipe, void* uboData, Texture* texture) 
{
    MeshPrimitive* prim;
    if (!mesh || !pipe || !uboData || !texture)
    {
        slog("missing an argument for mesh_queue_render!");
        return;
    }

    int count;
    count = gfc_list_count(mesh->primitives);

    for (int i = 0; i < count; i++)
    {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;

        gf3d_pipeline_queue_render(pipe, prim->vertexBuffer, prim->vertexCount, prim->faceBuffer, uboData, texture);
    }
}

