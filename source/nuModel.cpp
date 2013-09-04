#include "nuModel.h"
#include "nuDefs.h"
#include "nuContext.h"
#include "nuGraphics.h"

using namespace Nultima;

Model::Model(int type) :
    m_indexBuffer(0),
    m_vertexBuffer(0),
    m_numTriangles(0)
{
    switch (type)
    {
    case UNIT_PLANE:
        createUnitPlane();
        break;

    case UNIT_BOX:
        createUnitBox();
        break;

    default:
        NU_ASSERT(!"Unknown model type");
    }
}

void Model::createUnitPlane()
{
    std::vector<Vec3> verts;
    std::vector<Vec3ui> tris;

    verts.push_back(Vec3(0, 0, 0));
    verts.push_back(Vec3(1, 0, 0));
    verts.push_back(Vec3(1, 1, 0));
    verts.push_back(Vec3(0, 1, 0));

    tris.push_back(Vec3ui(0, 1, 2));
    tris.push_back(Vec3ui(0, 2, 3));

    // TODO [sampo] Normals?

    Graphics* graphics = Context::get()->getGraphics();
    m_numTriangles = tris.size();
    m_indexBuffer = graphics->createIndexBuffer(&tris[0].m_x, tris.size());
    m_vertexBuffer = graphics->createVertexBuffer(&verts[0].m_x, verts.size());
}

void Model::createUnitBox()
{
    NU_ASSERT(!"Not implemented");
}

void Model::render() const
{
    Graphics* graphics = Context::get()->getGraphics();
    graphics->bindIndexBuffer(m_indexBuffer);
    graphics->bindVertexBuffer(m_vertexBuffer);
    graphics->drawElements(m_numTriangles);
}
