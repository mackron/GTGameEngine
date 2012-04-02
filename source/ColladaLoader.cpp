
#include <GTEngine/ColladaLoader.hpp>
#include <GTEngine/SceneNode.hpp>

#include <GTCore/Vector.hpp>
#include <GTCore/String.hpp>
#include <GTCore/Parse.hpp>
#include <GTCore/Strings/Tokenizer.hpp>

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <rapidxml/rapidxml.hpp>
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
using namespace rapidxml;

namespace GTEngine
{
    glm::vec3 PositionFromXUp(float x, float y, float z)
    {
        return glm::vec3(z, y, -x);
    }

    glm::vec3 PositionFromYUp(float x, float y, float z)
    {
        return glm::vec3(x, y, -z);
    }

    glm::vec3 PositionFromZUp(float x, float y, float z)
    {
        return glm::vec3(x, z, -y);
    }


    glm::vec3 RotationAxisFromXUp(float x, float y, float z)
    {
        return glm::vec3(z, y, -x);
    }

    glm::vec3 RotationAxisFromYUp(float x, float y, float z)
    {
        return glm::vec3(x, y, -z);
    }

    glm::vec3 RotationAxisFromZUp(float x, float y, float z)
    {
        return glm::vec3(x, z, -y);
    }

    glm::quat QuaternionFromXUp(const glm::quat &q)
    {
        return glm::quat(q.w, q.z, q.y, -q.x);
    }

    glm::quat QuaternionFromYUp(const glm::quat &q)
    {
        return glm::quat(q.w, q.x, q.y, -q.z);
    }

    glm::quat QuaternionFromZUp(const glm::quat &q)
    {
        return glm::quat(q.w, q.x, q.z, -q.y);
    }


    glm::vec3 ScaleFromXUp(float x, float y, float z)
    {
        return glm::vec3(z, y, x);
    }

    glm::vec3 ScaleFromYUp(float x, float y, float z)
    {
        return glm::vec3(x, y, z);
    }

    glm::vec3 ScaleFromZUp(float x, float y, float z)
    {
        return glm::vec3(x, z, y);
    }


    // Format: %f %f %f
    void Collada_ToVec3(const char *value, float &x, float &y, float &z)
    {
        char temp[64];
        GTCore::Strings::Tokenizer token(value);

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        x = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        y = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        z = GTCore::Parse<float>(temp);
    }

    // Format: %f %f %f %f
    void Collada_ToVec4(const char *value, float &x, float &y, float &z, float &w)
    {
        char temp[64];
        GTCore::Strings::Tokenizer token(value);

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        x = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        y = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        z = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        w = GTCore::Parse<float>(temp);
    }

    void Collada_ToFloat3x3(const char *value, glm::mat3x3 &dest)
    {
        char temp[64];
        GTCore::Strings::Tokenizer token(value);

        // Row 0
        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[0][0] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[0][1] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[0][2] = GTCore::Parse<float>(temp);
        ++token;

        // Row 1
        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[1][0] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[1][1] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[1][2] = GTCore::Parse<float>(temp);
        ++token;

        // Row 2
        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[2][0] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[2][1] = GTCore::Parse<float>(temp);
        ++token;

        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
        dest[2][2] = GTCore::Parse<float>(temp);
    }


    /**
    *   \brief  Structure containing the data from the first pass of the collada file.
    *
    *   We only store values from the common technique and profile.
    */
    struct Collada
    {
        // <asset>
        struct _asset
        {
            // Y_UP by default, as noted in the specification.
            _asset(xml_node<> *node)
                : meter(1.0f), upAxis(1)
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "unit"))
                    {
                        this->meter = GTCore::Parse<float>(child->first_attribute("meter")->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "up_axis"))
                    {
                        if (GTCore::Strings::Equal(child->value(), "X_UP"))
                        {
                            this->upAxis = 0;
                        }
                        else if (GTCore::Strings::Equal(child->value(), "Z_UP"))
                        {
                            this->upAxis = 2;
                        }
                        else
                        {
                            // Y_UP by default.
                            this->upAxis = 1;
                        }
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            // Constructor for copying an asset from a pointer.
            _asset(_asset *other)
                : meter(other->meter), upAxis(other->upAxis)
            {
            }
            
            /**
            *   \brief  Converts a position to Z-out orientation (+Z towards the viewer, OpenGL orientation).
            */
            glm::vec4 position_to_zout(float x, float y, float z, float w)
            {
                if (this->upAxis == 0)      // X_UP
                {
                    return glm::vec4(z, y, -x, w);
                }
                else if (this->upAxis == 2) // Z_UP
                {
                    return glm::vec4(x, z, -y, w);
                }
                else                        // Y_UP (Default)
                {
                    return glm::vec4(x, y, -z, w);
                }
            }
            
            /**
            *   \brief  Converts an orientation to Z-out orientation (+Z towards the viewer, OpenGL orientation).
            */
            glm::quat orientation_to_zout(const glm::quat &q)
            {
                if (this->upAxis == 0)      // X_UP
                {
                    return glm::quat(q.w, q.z, q.y, -q.x);
                }
                else if (this->upAxis == 2) // Z_UP
                {
                    return glm::quat(q.w, q.x, q.z, -q.y);
                }
                else                        // Y_UP (Default)
                {
                    return glm::quat(q.w, q.x, q.y, -q.z);
                }
            }
            
            /**
            *   \brief  Converts a scale to Z-out orientations (+Z towards the viewer, OpenGL orientation).
            */
            glm::vec3 scale_to_zout(float x, float y, float z)
            {
                if (this->upAxis == 0)      // X_UP
                {
                    return glm::vec3(z, y, x);
                }
                else if (this->upAxis == 2) // Z_UP
                {
                    return glm::vec3(x, z, y);
                }
                else                        // Y_UP (Default)
                {
                    return glm::vec3(x, y, z);
                }
            }
            
            float meter;    // The size of 1 meter. In our engine, 1 unit is 1 meter.
            int   upAxis;   // 0 = X_UP, 1 = Y_UP, 2 = Z_UP.
        };
        
        // Enumerator for the different transform types.
        enum transform_type
        {
            transform_type_none,
            transform_type_translate,
            transform_type_rotate,
            transform_type_scale,
            transform_type_lookat
        };
        
        // Base class for transforms (<translate>, <rotate>, <scale>).
        struct _transform
        {
            _transform(transform_type type)
                : type(type)
            {
            }

            virtual ~_transform()
            {
            }
            
            transform_type type;
        };
        
        // <translate>
        struct _translate : public _transform
        {
            _translate(xml_node<> *node)
                : _transform(transform_type_translate), x(0), y(0), z(0)
            {
                if (node)
                {
                    Collada_ToVec3(node->value(), this->x, this->y, this->z);
                }
            }
            
            float x, y, z;
        };
        
        // <rotate>
        struct _rotate : public _transform
        {
            _rotate(xml_node<> *node)
                : _transform(transform_type_rotate), x(0), y(0), z(0), angle(0)
            {
                if (node)
                {
                    Collada_ToVec4(node->value(), this->x, this->y, this->z, this->angle);
                }
            }
            
            float x, y, z;
            float angle;
        };
        
        // <scale>
        struct _scale : public _transform
        {
            _scale(xml_node<> *node)
                : _transform(transform_type_scale), x(1), y(1), z(1)
            {
                if (node)
                {
                    Collada_ToVec3(node->value(), this->x, this->y, this->z);
                }
            }
            
            float x, y, z;
        };
        
        // <lookat>
        struct _lookat : public _transform
        {
            _lookat(xml_node<> *node)
                : _transform(transform_type_lookat),
                  px(),  py(),  pz(),
                  ix(),  iy(),  iz(),
                  upx(), upy(), upz()
            {
                if (node != nullptr)
                {
                    glm::mat3x3 value;
                    Collada_ToFloat3x3(node->value(), value);

                    this->px = value[0][0];
                    this->py = value[0][1];
                    this->pz = value[0][2];

                    this->ix = value[1][0];
                    this->iy = value[1][1];
                    this->iz = value[1][2];

                    this->upx = value[2][0];
                    this->upy = value[2][1];
                    this->upz = value[2][2];
                }
                else
                {
                    this->px = 0.0f;
                    this->py = 0.0f;
                    this->pz = 0.0f;

                    this->ix = 0.0f;
                    this->iy = 0.0f;
                    this->iz = 0.0f;

                    this->upx = 0.0f;
                    this->upy = 0.0f;
                    this->upz = 0.0f;
                }
            }

            _lookat()
                : _transform(transform_type_lookat),
                  px(0.0f),  py(0.0f),  pz(0.0f),
                  ix(0.0f),  iy(0.0f),  iz(0.0f),
                  upx(0.0f), upy(0.0f), upz(0.0f)
            {
            }
            
            float px,  py,  pz;
            float ix,  iy,  iz;
            float upx, upy, upz;
        };
        
        
        // <perspective>
        struct _perspective
        {
            _perspective(xml_node<> *node)
                : xfov(0.0f), yfov(0.0f), aspect_ratio(0.0f), znear(0.0f), zfar(0.0f)
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "xfov"))
                    {
                        this->xfov = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "yfov"))
                    {
                        this->yfov = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "aspect_ratio"))
                    {
                        this->aspect_ratio = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "znear"))
                    {
                        this->znear = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "zfar"))
                    {
                        this->zfar = GTCore::Parse<float>(child->value());
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            float xfov;
            float yfov;
            float aspect_ratio;
            float znear;
            float zfar;
        };
        
        // <orthographic>
        struct _orthographic
        {
            _orthographic(xml_node<> *node)
                : xmag(0.0f), ymag(0.0f), aspect_ratio(0.0f), znear(0.0f), zfar(0.0f)
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "xmag"))
                    {
                        this->xmag = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "ymag"))
                    {
                        this->ymag = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "aspect_ratio"))
                    {
                        this->aspect_ratio = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "znear"))
                    {
                        this->znear = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Equal(child->name(), "zfar"))
                    {
                        this->zfar = GTCore::Parse<float>(child->value());
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            float xmag;
            float ymag;
            float aspect_ratio;
            float znear;
            float zfar;
        };
        
        // <optics>
        struct _optics
        {
            _optics(xml_node<> *node)
                : perspective(nullptr), orthographic(nullptr)
            {
                // We only care about the common technique. If we don't have it, it's invalid and we silently ignore.
                xml_node<> *technique_node = node->first_node("technique_common");
                if (technique_node)
                {
                    // We will have either a perspective or orthographic tag next.
                    xml_node<> *projection_node = technique_node->first_node("perspective");
                    if (projection_node != nullptr)
                    {
                        // We have a perspective node.
                        this->perspective = new Collada::_perspective(projection_node);
                    }
                    else
                    {
                        projection_node = technique_node->first_node("orthographic");
                        if (projection_node != nullptr)
                        {
                            // We have an orthographic node.
                            this->orthographic = new Collada::_orthographic(projection_node);
                        }
                    }
                }
            }
            
            ~_optics()
            {
                delete perspective;
                delete orthographic;
            }
            
            _perspective *perspective;
            _orthographic *orthographic;

        private:    // No copying.
            _optics(const _optics &);
            _optics & operator=(const _optics &);
        };
        
        // <camera>
        struct _camera
        {
            _camera(xml_node<> *node, _asset *parentAsset)
                : id(), name(), asset(nullptr), optics(nullptr)
            {
                // Attributes.
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "id"))
                    {
                        this->id = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "name"))
                    {
                        this->name = attr->name();
                    }

                    attr = attr->next_attribute(nullptr);
                }
                

                // Children.
                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *opticsNode = node->first_node("optics");
                if (opticsNode)
                {
                    this->optics = new _optics(opticsNode);
                }
            }
            
            ~_camera()
            {
                delete asset;
                delete optics;
            }
            
            GTCore::String id;
            GTCore::String name;
            
            _asset  *asset;
            _optics *optics;

        private:    // No copying.
            _camera(const _camera &);
            _camera & operator=(const _camera &);
        };
        
        
        // <instance_camera>
        struct _instance_camera
        {
            _instance_camera(xml_node<> *node)
                : url()
            {
                xml_attribute<> *attr = node->first_attribute("url");
                if (attr)
                {
                    this->url = attr->value();
                }
            }
            
            GTCore::String url;
        };
        
        // <instance_light>
        struct _instance_light
        {
            _instance_light(xml_node<> *node)
                : url()
            {
                xml_attribute<> *attr = node->first_attribute("url");
                if (attr)
                {
                    this->url = attr->value();
                }
            }
            
            GTCore::String url;
        };
        
        // <instance_geometry>
        struct _instance_geometry
        {
            _instance_geometry(xml_node<> *node)
                : url()
            {
                // TODO: Still need to do materials.
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "url"))
                    {
                        this->url = attr->value();
                    }

                    attr = attr->next_attribute(nullptr);
                }
            }
            
            GTCore::String url;
            
            // TODO: Materials.
        };
        
        // <node>
        struct _node
        {
            _node(xml_node<> *node, _asset *parentAsset)
                : id(), type("NODE"), asset(nullptr), transforms(), cameras(), lights(), geometries(), nodes()
            {
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "id"))
                    {
                        this->id = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "type"))
                    {
                        this->type = attr->value();
                    }

                    attr = attr->next_attribute(nullptr);
                }


                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "translate"))
                    {
                        this->transforms.Append(new _translate(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "rotate"))
                    {
                        this->transforms.Append(new _rotate(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "scale"))
                    {
                        this->transforms.Append(new _scale(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "lookat"))
                    {
                        this->transforms.Append(new _lookat(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "instance_camera"))
                    {
                        this->cameras.Append(new _instance_camera(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "instance_light"))
                    {
                        this->lights.Append(new _instance_light(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "instance_geometry"))
                    {
                        this->geometries.Append(new _instance_geometry(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "node"))
                    {
                        this->nodes.Append(new _node(child, this->asset));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_node()
            {
                delete asset;
                
                while (transforms.root != nullptr)
                {
                    delete (_transform *)transforms.root->value;
                    transforms.RemoveRoot();
                }
                
                while (cameras.root != nullptr)
                {
                    delete (_instance_camera *)cameras.root->value;
                    cameras.RemoveRoot();
                }

                while (lights.root != nullptr)
                {
                    delete (_instance_light *)lights.root->value;
                    lights.RemoveRoot();
                }

                while (geometries.root != nullptr)
                {
                    delete (_instance_geometry *)geometries.root->value;
                    geometries.RemoveRoot();
                }

                while (nodes.root != nullptr)
                {
                    delete (_node *)nodes.root->value;
                    nodes.RemoveRoot();
                }
            }
            
            GTCore::String id;
            GTCore::String type;
            
            _asset *asset;
            
            // The list of transform in the node, in the order they appear. Order is important.
            GTCore::List<_transform *> transforms;
            
            // The list of camera instances for this node.
            GTCore::List<_instance_camera *> cameras;
            
            // The list of light instances for this node.
            GTCore::List<_instance_light *> lights;
            
            // The list of geometries in this node.
            GTCore::List<_instance_geometry *> geometries;
            
            // The list of child nodes.
            GTCore::List<_node *> nodes;

        private:    // No copying.
            _node(const _node &);
            _node & operator=(const _node &);
        };
        
        
        // <library_cameras>
        struct _library_cameras
        {
            _library_cameras(xml_node<> *node, _asset *parentAsset)
                : asset(nullptr), cameras()
            {
                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "camera"))
                    {
                        this->cameras.Append(new _camera(child, this->asset));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_library_cameras()
            {
                delete asset;
                
                while (cameras.root != nullptr)
                {
                    delete cameras.root->value;
                    cameras.RemoveRoot();
                }
            }
            
            _camera * find(const GTCore::String &id)
            {
                for (auto i = this->cameras.root; i != nullptr; i = i->next)
                {
                    auto iCamera = i->value;

                    if (Collada::CompareURLToID(id, iCamera->id))
                    {
                        return iCamera;
                    }
                }
                
                return nullptr;
            }
            
            // The asset for this library.
            _asset *asset;
            
            // The list of cameras in this library.
            GTCore::List<_camera *> cameras;

        private:    // No copying.
            _library_cameras(const _library_cameras &);
            _library_cameras & operator=(const _library_cameras &);
        };
        
        
        // <point>
        struct _point
        {
            _point(xml_node<> *node)
                : colourR(1.0f), colourG(1.0f), colourB(1.0f),
                  constantAttenuation(1.0f), linearAttenuation(0.0f), quadraticAttenuation(0.0666f)
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Compare(child->name(), "colour"))
                    {
                        char temp[64];

                        GTCore::Strings::Tokenizer token(child->value());

                        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                        colourR = GTCore::Parse<float>(temp);
                        ++token;

                        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                        colourG = GTCore::Parse<float>(temp);
                        ++token;

                        GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                        colourB = GTCore::Parse<float>(temp);
                    }
                    else if (GTCore::Strings::Compare(child->name(), "constant_attenuation"))
                    {
                        constantAttenuation = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Compare(child->name(), "linear_attenuation"))
                    {
                        linearAttenuation = GTCore::Parse<float>(child->value());
                    }
                    else if (GTCore::Strings::Compare(child->name(), "quadratic_attenuation"))
                    {
                        quadraticAttenuation = GTCore::Parse<float>(child->value());
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            float colourR, colourG, colourB;
            float constantAttenuation;
            float linearAttenuation;
            float quadraticAttenuation;
        };
        
        // <light>
        struct _light
        {
            _light(xml_node<> *node)
                : id(), name(), point(nullptr)
            {
                // Attributes.
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "id"))
                    {
                        this->id = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "name"))
                    {
                        this->name = attr->value();
                    }

                    attr = attr->next_attribute(nullptr);
                }

                // Children.
                xml_node<> *technique_node = node->first_node("technique_common");
                if (technique_node)
                {
                    xml_node<> *child = technique_node->first_node();
                    while (child)
                    {
                        if (GTCore::Strings::Equal(child->name(), "point"))
                        {
                            this->point = new _point(child);
                        }
                        
                        child = child->next_sibling();
                    }
                }
            }
            
            ~_light()
            {
                delete point;
            }
            
            GTCore::String id;
            GTCore::String name;
            
            _point *point;

        private:    // No copying.
            _light(const _light &);
            _light & operator=(const _light &);
        };
        
        // <library_lights>
        struct _library_lights
        {
            _library_lights(xml_node<> *node)
                : lights()
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "light"))
                    {
                        this->lights.Append(new _light(child));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_library_lights()
            {
                while (lights.root != nullptr)
                {
                    delete lights.root->value;
                    lights.RemoveRoot();
                }
            }
            
            _light * find(const GTCore::String &id)
            {
                for (auto i = this->lights.root; i != nullptr; i = i->next)
                {
                    auto iLight = i->value;

                    if (Collada::CompareURLToID(id, iLight->id))
                    {
                        return iLight;
                    }
                }
                
                return nullptr;
            }
            
            // The list of lights in this library.
            GTCore::List<_light *> lights;
        };
        
        
        // <params>
        struct _param
        {
            _param(xml_node<> *node)
                : name(), type()
            {
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "name"))
                    {
                        this->name = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "type"))
                    {
                        this->type = attr->value();
                    }

                    attr = attr->next_attribute(nullptr);
                }
            }
            
            GTCore::String name;
            GTCore::String type;
        };
        
        // <accessor>
        struct _accessor
        {
            _accessor(xml_node<> *node)
                : source(), count(0), stride(0), params()
            {
                // Attributes.
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "source"))
                    {
                        this->source = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "count"))
                    {
                        this->count = GTCore::Parse<size_t>(attr->value());
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "stride"))
                    {
                        this->stride = GTCore::Parse<size_t>(attr->value());
                    }

                    attr = attr->next_attribute(nullptr);
                }

                // Children.
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "param"))
                    {
                        // We have found a camera. We need the ID and name attributes of the camera.
                        this->params.Append(new _param(child));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_accessor()
            {
                while (params.root != nullptr)
                {
                    delete params.root->value;
                    params.RemoveRoot();
                }
            }
            
            GTCore::String source;
            size_t count;
            size_t stride;
            
            GTCore::List<_param *> params;
        };
        
        // <float_array>
        struct _float_array
        {
            _float_array(xml_node<> *node)
                : id(), count(0), values()
            {
                // Attributes.
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "id"))
                    {
                        this->id = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "count"))
                    {
                        this->count = GTCore::Parse<size_t>(attr->value());
                    }

                    attr = attr->next_attribute(nullptr);
                }


                // This will not have children, so we just parse the value directly.
                char temp[64];

                GTCore::Strings::Tokenizer token(node->value());
                while (token)
                {
                    GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                    values.PushBack(GTCore::Parse<float>(temp));

                    ++token;
                }
            }
            
            GTCore::String id;
            
            // The count. Isn't really used.
            size_t count;
            
            // The list of floats. It's easiest to keep this with random access.
            GTCore::Vector<float> values;
        };
        
        // <source>
        struct _source
        {
            _source(xml_node<> *node)
                : id(), float_array(nullptr), accessor(nullptr)
            {
                xml_attribute<> *attr = node->first_attribute("id");
                if (attr != nullptr)
                {
                    this->id = attr->value();
                }

                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "float_array"))
                    {
                        // We have found a camera. We need the ID and name attributes of the camera.
                        this->float_array = new _float_array(child);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "technique_common"))
                    {
                        xml_node<> *accessorNode = child->first_node("accessor");
                        if (accessorNode)
                        {
                            this->accessor = new _accessor(accessorNode);
                        }
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_source()
            {
                delete float_array;
                delete accessor;
            }
            
            GTCore::String id;
            
            // The float array, if any.
            _float_array *float_array;
            
            // The accessor.
            _accessor *accessor;

        private:    // No copying.
            _source(const _source &);
            _source & operator=(const _source &);
        };
        
        // <input>
        struct _input
        {
            _input(xml_node<> *node)
                : semantic(), source(), offset(0)
            {
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "semantic"))
                    {
                        this->semantic = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "source"))
                    {
                        this->source = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "offset"))
                    {
                        this->offset = GTCore::Parse<size_t>(attr->value());
                    }
                    
                    attr = attr->next_attribute(nullptr);
                }
            }
            
            GTCore::String semantic;
            GTCore::String source;
            size_t offset;
        };
        
        // <vertices>
        struct _vertices
        {
            _vertices(xml_node<> *node)
                : id(), input(nullptr)
            {
                xml_attribute<> *attr = node->first_attribute("id");
                if (attr != nullptr)
                {
                    this->id = attr->value();
                }

                xml_node<> *child = node->first_node("input");
                if (child)
                {
                    input = new _input(child);
                }
            }
            
            ~_vertices()
            {
                delete input;
            }
            
            GTCore::String id;
            _input *input;

        private:    // No copying.
            _vertices(const _vertices &);
            _vertices & operator=(const _vertices &);
        };
        
        // <p>
        struct _p
        {
            _p(xml_node<> *node)
                : indices()
            {
                char temp[64];

                GTCore::Strings::Tokenizer token(node->value());
                while (token)
                {
                    GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                    indices.PushBack(GTCore::Parse<unsigned int>(temp));

                    ++token;
                }
            }
            
            GTCore::Vector<unsigned int> indices;   // <-- Needs random access, so can't use a GTCore::List.
        };
        
        // <vcount>
        struct _vcount
        {
            _vcount(xml_node<> *node)
                : counts()
            {
                char temp[64];

                GTCore::Strings::Tokenizer token(node->value());
                while (token)
                {
                    GTCore::Strings::Copy(temp, 64, token.start, token.GetSizeInTs());
                    counts.Append(GTCore::Parse<unsigned int>(temp));

                    ++token;
                }
            }
            
            GTCore::List<unsigned int> counts;  // unsigned int
        };
        
        // Enumerator for the different primitive types.
        enum primitive_type
        {
            primitive_type_none,
            primitive_type_polylist,
            primitive_type_triangles,

            // TODO: Need to do the rest.
        };
        
        // Base class for primitives (polylist, triangles, etc).
        struct _primitive
        {
            _primitive(primitive_type type)
                : type(type)
            {
            }

            virtual ~_primitive()
            {
            }
            
            primitive_type type;
        };
        
        
        // <polylist>
        struct _polylist : public _primitive
        {
            _polylist(xml_node<> *node)
                : _primitive(primitive_type_polylist), material(), count(0), inputs(), vcount(nullptr), p(nullptr)
            {
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "material"))
                    {
                        this->material = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "count"))
                    {
                        this->count = GTCore::Parse<size_t>(attr->value());
                    }

                    attr = attr->next_attribute(nullptr);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "input"))
                    {
                        // We have found a camera. We need the ID and name attributes of the camera.
                        this->inputs.Append(new _input(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "vcount"))
                    {
                        // We have found a camera. We need the ID and name attributes of the camera.
                        this->vcount = new _vcount(child);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "p"))
                    {
                        // We have found a camera. We need the ID and name attributes of the camera.
                        this->p = new _p(child);
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_polylist()
            {
                while (this->inputs.root != nullptr)
                {
                    delete this->inputs.root->value;
                    this->inputs.RemoveRoot();
                }
                
                delete vcount;
                delete p;
            }
            
            GTCore::String material;
            size_t count;
            
            // The list of inputs.
            GTCore::List<_input *> inputs;
            
            _vcount *vcount;
            _p *p;

        private:    // No copying.
            _polylist(const _polylist &);
            _polylist & operator=(const _polylist &);
        };
        
        // <mesh>
        struct _mesh
        {
            _mesh(xml_node<> *node)
                : sources(), vertices(nullptr), primitives()
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "source"))
                    {
                        this->sources.Append(new _source(child));
                    }
                    else if (GTCore::Strings::Equal(child->name(), "vertices"))
                    {
                        this->vertices = new _vertices(child);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "polylist"))
                    {
                        this->primitives.Append(new _polylist(child));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_mesh()
            {
                while (this->sources.root != nullptr)
                {
                    delete this->sources.root->value;
                    this->sources.RemoveRoot();
                }
                
                while (this->primitives.root != nullptr)
                {
                    delete this->primitives.root->value;
                    this->primitives.RemoveRoot();
                }

                delete vertices;
            }
            
            _source * find_source(const GTCore::String &url)
            {
                for (auto i = this->sources.root; i != nullptr; i = i->next)
                {
                    auto iSource = i->value;
                    if (iSource != nullptr && Collada::CompareURLToID(url, iSource->id))
                    {
                        return iSource;
                    }
                }
                
                return nullptr;
            }
            
            // The sources of the mesh.
            GTCore::List<_source *> sources;
            
            _vertices *vertices;
            GTCore::List<_primitive *> primitives;

        private:    // No copying.
            _mesh(const _mesh &);
            _mesh & operator=(const _mesh &);
        };
        
        // <geometry>
        struct _geometry
        {
            _geometry(xml_node<> *node, _asset *parentAsset)
                : id(), name(), asset(nullptr), mesh(nullptr), meshes()
            {
                // Attributes.
                xml_attribute<> *attr = node->first_attribute("id");
                if (attr != nullptr)
                {
                    this->id = attr->value();
                }

                attr = node->first_attribute("name");
                if (attr != nullptr)
                {
                    this->name = attr->value();
                }
                

                // Children.
                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "mesh"))
                    {
                        this->mesh = new _mesh(child);
                    }
                    
                    child = child->next_sibling();
                }
                
                // We need to build the meshes from the mesh primitives.
                this->build_meshes();
            }
            
            ~_geometry()
            {
                delete asset;
                delete mesh;
            }
            
            void build_meshes()
            {
                if (this->mesh)
                {
                    // We create a new mesh for each primitive.
                    for (auto i = this->mesh->primitives.root; i != nullptr; i = i->next)
                    {
                        auto primitive = (Collada::_primitive *)i->value;
                        
                        Collada::_source *positionsSource = nullptr;
                        size_t positionsOffset = 0;
                        
                        Collada::_source *normalsSource = nullptr;
                        size_t normalsOffset = 0;
                        
                        Collada::_source *texCoordSource = nullptr;
                        size_t texCoordOffset = 0;
                        
                        VertexArray* newMesh = nullptr;

                        // Now we need the inputs for the mesh.
                        switch (primitive->type)
                        {
                            case Collada::primitive_type_polylist:
                            {
                                Collada::_polylist *polylist = (Collada::_polylist *)primitive;
                                
                                // The format of our mesh data.
                                VertexFormat vertexFormat;
                                
                                for (auto j = polylist->inputs.root; j != nullptr; j = j->next)
                                {
                                    auto input = (Collada::_input *)j->value;
                                    
                                    if (GTCore::Strings::Equal(input->semantic.c_str(), "VERTEX"))
                                    {
                                        // These are just the positions.
                                        if (this->mesh->vertices != nullptr && this->mesh->vertices->input != nullptr)
                                        {
                                            positionsSource = this->mesh->find_source(this->mesh->vertices->input->source);
                                            positionsOffset = input->offset;
                                        
                                            vertexFormat.AddAttribute(VertexAttribs::Position, static_cast<int>(positionsSource->accessor->stride));
                                        }
                                    }
                                    else if (GTCore::Strings::Equal(input->semantic.c_str(),"NORMAL"))
                                    {
                                        normalsSource = this->mesh->find_source(input->source);
                                        normalsOffset = input->offset;
                                        
                                        vertexFormat.AddAttribute(VertexAttribs::Normal, static_cast<int>(normalsSource->accessor->stride));
                                    }
                                    else if (GTCore::Strings::Equal(input->semantic.c_str(), "TEXCOORD"))
                                    {
                                        texCoordSource = this->mesh->find_source(input->source);
                                        texCoordOffset = input->offset;
                                        
                                        vertexFormat.AddAttribute(VertexAttribs::TexCoord, static_cast<int>(texCoordSource->accessor->stride));
                                    }
                                }

                                // We also need to allocate for tangents and bitangents. These will be calculated later.
                                vertexFormat.AddAttribute(VertexAttribs::Tangent,   3);
                                vertexFormat.AddAttribute(VertexAttribs::Bitangent, 3);

                                
                                // We can now grab the size of an element for future use.
                                size_t elementSize = vertexFormat.GetSize();
                                
                                // The list of elements.
                                GTCore::Vector<float> elements;
                                
                                // The list of indices.
                                GTCore::Vector<unsigned int> indices;
                                
                                // The current 'p' index we're looking at.
                                size_t pIndex = 0;
                                
                                // Primitives aren't neccessarilly triangles in a polylist. We need to convert them to trianges.
                                for (auto iCount = polylist->vcount->counts.root; iCount != nullptr; iCount = iCount->next)
                                {
                                    // We need to convert any quads to triangles. We are not supporting non-convex shapes.
                                    auto elementCount = iCount->value;
                                    for (unsigned int iElement = 0; iElement < elementCount; ++iElement)
                                    {
                                        // We will now grab the data for this element.
                                        float *element = new float[elementSize];
                                        
                                        if (positionsSource)
                                        {
                                            // TODO: We need to change these to use the selector.
                                            size_t positionComponentOffset = static_cast<size_t>(vertexFormat.GetAttributeOffset(VertexAttribs::Position));
                                            size_t positionIndexOffset     = polylist->p->indices[pIndex + positionsOffset] * positionsSource->accessor->stride;
                                            
                                            float posX = positionsSource->float_array->values[positionIndexOffset + 0];
                                            float posY = positionsSource->float_array->values[positionIndexOffset + 1];
                                            float posZ = positionsSource->float_array->values[positionIndexOffset + 2];
                                            
                                            // The position needs to be converted.
                                            glm::vec4 position = this->asset->position_to_zout(posX, posY, posZ, 1.0f);
                                            
                                            element[positionComponentOffset + 0] = position.x;
                                            element[positionComponentOffset + 1] = position.y;
                                            element[positionComponentOffset + 2] = position.z;
                                        }
                                        
                                        if (normalsSource)
                                        {
                                            size_t normalComponentOffset = static_cast<size_t>(vertexFormat.GetAttributeOffset(VertexAttribs::Normal));
                                            size_t normalIndexOffset     = polylist->p->indices[pIndex + normalsOffset] * normalsSource->accessor->stride;
                                            
                                            float x = normalsSource->float_array->values[normalIndexOffset + 0];
                                            float y = normalsSource->float_array->values[normalIndexOffset + 1];
                                            float z = normalsSource->float_array->values[normalIndexOffset + 2];
                                            
                                            // The normal needs to be converted.
                                            glm::vec4 normal = this->asset->position_to_zout(x, y, z, 1.0f);
                                            
                                            element[normalComponentOffset + 0] = normal.x;
                                            element[normalComponentOffset + 1] = normal.y;
                                            element[normalComponentOffset + 2] = normal.z;
                                        }
                                        
                                        if (texCoordSource)
                                        {
                                            size_t texCoordComponentOffset = static_cast<size_t>(vertexFormat.GetAttributeOffset(VertexAttribs::TexCoord));
                                            size_t texCoordIndexOffset     = polylist->p->indices[pIndex + texCoordOffset] * texCoordSource->accessor->stride;
                                            
                                            float s = texCoordSource->float_array->values[texCoordIndexOffset + 0];
                                            float t = texCoordSource->float_array->values[texCoordIndexOffset + 1];
                                            
                                            element[texCoordComponentOffset + 0] = s;
                                            element[texCoordComponentOffset + 1] = t;
                                        }
                                        
                                        
                                        // We can now add the element to the list.
                                        size_t newIndex = elements.count / elementSize;
                                        for (size_t i = 0; i < elementSize; ++i)
                                        {
                                            elements.PushBack(element[i]);
                                        }
                                        
                                        delete element;
                                        
                                        // Now we need to push indices. If we're still on the first triangle, we just push the new index. Otherwise
                                        // we push the first index for this polygon, then the previously added index and then the new index.
                                        if (iElement <= 2)
                                        {
                                            indices.PushBack(static_cast<unsigned int>(newIndex));
                                        }
                                        else
                                        {
                                            size_t firstIndex = indices[indices.count - iElement];
                                            size_t lastIndex  = indices.GetBack();
                                            
                                            indices.PushBack(static_cast<unsigned int>(firstIndex));
                                            indices.PushBack(static_cast<unsigned int>(lastIndex));
                                            indices.PushBack(static_cast<unsigned int>(newIndex));
                                        }
                                        
                                        pIndex += polylist->inputs.Count();
                                    }
                                }

                                // Now we can create our new mesh and set the data.
                                newMesh = new VertexArray(VertexArrayUsage_Static, vertexFormat);
                                newMesh->SetData(&elements[0], elements.count / vertexFormat.GetSize(), &indices[0], indices.count);

                                break;
                            }
                            
                            case Collada::primitive_type_none:
                            case Collada::primitive_type_triangles:
                            default: break;
                        }
                        
                        // Now we need to add the mesh to the list, if we have one.
                        if (newMesh != nullptr)
                        {
                            this->meshes.Append(newMesh);
                        }
                    }
                }
            }
            
            GTCore::String id;
            GTCore::String name;
            
            // The asset tag. This is set during the constructor. If the element does not have an asset element, it inherits the
            // parent asset. Otherwise it uses it's own.
            _asset *asset;
            
            // The mesh data of the geometry.
            _mesh *mesh;
            
            // The list of Mesh's making up the geometry. There is one of these for each primitive in 'mesh'. They are constructed
            // in the constructor after all of the primitives have been loaded. The name is based on the meshes index and the ID of
            // the geometry.
            GTCore::List<VertexArray *> meshes;

        private:    // No copying.
            _geometry(const _geometry &);
            _geometry & operator=(const _geometry &);
        };
        
        // <library_geometries>
        struct _library_geometries
        {
            _library_geometries(xml_node<> *node, _asset *parentAsset)
                : geometries(), asset(nullptr)
            {
                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "geometry"))
                    {
                        this->geometries.Append(new _geometry(child, this->asset));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_library_geometries()
            {
                delete asset;
                
                while (this->geometries.root != nullptr)
                {
                    delete this->geometries.root->value;
                    this->geometries.RemoveRoot();
                }
            }
            
            _geometry * find(const GTCore::String &id)
            {
                for (auto i = geometries.root; i != nullptr; i = i->next)
                {
                    auto iGeometry = i->value;

                    if (Collada::CompareURLToID(id, iGeometry->id))
                    {
                        return iGeometry;
                    }
                }
                
                return nullptr;
            }
            
            // The list of geometries in the library.
            GTCore::List<_geometry *> geometries;    // _geometry*
            
            // The asset inheritted from the parent and used by it's children.
            _asset *asset;

        private:    // No copying.
            _library_geometries(const _library_geometries &);
            _library_geometries & operator=(const _library_geometries &);
        };
        
        
        // <instance_material>
        struct _instance_material
        {
            _instance_material(xml_node<> *node)
                : symbol(), target()
            {
                xml_attribute<> *attr = nullptr;
                if ((attr = node->first_attribute("symbol")))
                {
                    this->symbol = attr->value();
                }
                if ((attr = node->first_attribute("target")))
                {
                    this->target = attr->value();
                }
            }
            
            GTCore::String symbol;
            GTCore::String target;
        };
        
        // <bind_material>
        struct _bind_material
        {
            _bind_material(xml_node<> *node)
            {
                (void)node;
            }
        };
        
        
        
        
        // <visual_scene>
        struct _visual_scene
        {
            _visual_scene(xml_node<> *node, _asset *parentAsset)
                : id(), name(), asset(nullptr), nodes()
            {
                xml_attribute<> *attr = node->first_attribute();
                while (attr != nullptr)
                {
                    if (GTCore::Strings::Equal(attr->name(), "id"))
                    {
                        this->id = attr->value();
                    }
                    else if (GTCore::Strings::Equal(attr->name(), "name"))
                    {
                        this->name = attr->value();
                    }

                    attr = attr->next_attribute(nullptr);
                }


                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "node"))
                    {
                        this->nodes.Append(new _node(child, this->asset));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_visual_scene()
            {
                delete asset;
                
                while (this->nodes.root != nullptr)
                {
                    delete this->nodes.root->value;
                    this->nodes.RemoveRoot();
                }
            }
            
            GTCore::String id;
            GTCore::String name;
            
            _asset *asset;
            
            // The list of nodes in this visual scene.
            GTCore::List<_node *> nodes;


        private:    // No copying.
            _visual_scene(const _visual_scene &);
            _visual_scene & operator=(const _visual_scene &);
        };
        
        
        // <library_visual_scenes>
        struct _library_visual_scenes
        {
            _library_visual_scenes(xml_node<> *node, _asset *parentAsset)
                : asset(nullptr), visual_scenes()
            {
                xml_node<> *assetNode = node->first_node("asset");
                if (assetNode)
                {
                    this->asset = new _asset(assetNode);
                }
                else
                {
                    this->asset = new _asset(parentAsset);
                }
                
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "visual_scene"))
                    {
                        // We have found a visual scene. We need the ID and name attributes of the camera.
                        this->visual_scenes.Append(new _visual_scene(child, this->asset));
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_library_visual_scenes()
            {
                delete asset;
                
                while (this->visual_scenes.root != nullptr)
                {
                    delete this->visual_scenes.root->value;
                    this->visual_scenes.RemoveRoot();
                }
            }
            
            // Finds and returns the visual scene whose id matches the input id.
            _visual_scene * find(const GTCore::String &id)
            {
                for (auto i = this->visual_scenes.root; i != nullptr; i = i->next)
                {
                    auto iVisualScene = i->value;

                    if (Collada::CompareURLToID(id, iVisualScene->id))
                    {
                        return iVisualScene;
                    }
                }

                return nullptr;
            }
            
            // The asset for this library.
            _asset *asset;
            
            // The list of visual scenes in this library.
            GTCore::List<_visual_scene *> visual_scenes;

        private:    // No copying.
            _library_visual_scenes(const _library_visual_scenes &);
            _library_visual_scenes & operator=(const _library_visual_scenes &);
        };
        
        // <instance_visual_scene>
        struct _instance_visual_scene
        {
            _instance_visual_scene(xml_node<> *node)
                : url()
            {
                xml_attribute<> *attr = node->first_attribute("url");
                if (attr)
                {
                    this->url = attr->value();
                }
            }
            
            GTCore::String url;
        };
        
        // <scene>
        struct _scene
        {
            _scene(xml_node<> *node)
                : visual_scene(nullptr)
            {
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "instance_visual_scene"))
                    {
                        this->visual_scene = new _instance_visual_scene(child);
                    }
                    
                    child = child->next_sibling();
                }
            }
            
            ~_scene()
            {
                delete visual_scene;
            }
            
            // The visual scene, if we have one. Can only have 0 or 1.
            _instance_visual_scene *visual_scene;

        private:    // No copying.
            _scene(const _scene &);
            _scene & operator=(const _scene &);
        };
        
        
        Collada(xml_node<> *node)
            : asset(nullptr), library_cameras(nullptr), library_lights(nullptr), library_geometries(nullptr), library_visual_scenes(nullptr), scene(nullptr)
        {
            if (node != nullptr)
            {
                // Now lets iterate over every child.
                xml_node<> *child = node->first_node();
                while (child)
                {
                    if (GTCore::Strings::Equal(child->name(), "asset"))
                    {
                        this->asset = new _asset(child);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "library_cameras"))
                    {
                        this->library_cameras = new _library_cameras(child, this->asset);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "library_lights"))
                    {
                        this->library_lights = new _library_lights(child);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "library_geometries"))
                    {
                        this->library_geometries = new _library_geometries(child, this->asset);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "library_visual_scenes"))
                    {
                        this->library_visual_scenes = new _library_visual_scenes(child, this->asset);
                    }
                    else if (GTCore::Strings::Equal(child->name(), "scene"))
                    {
                        this->scene = new _scene(child);
                    }
                    
                    child = child->next_sibling();
                }
            }
        }
        
        ~Collada()
        {
            delete asset;
            delete library_cameras;
            delete library_lights;
            delete library_geometries;
            delete library_visual_scenes;
            delete scene;
        }
        
        
        // Compares a URL to an ID. Returns true if the URL is an ID and matches the given ID. If the URL is not
        // and ID, it will return false.
        static bool CompareURLToID(const GTCore::String &url, const GTCore::String &id)
        {
            if (IsURLToID(url))
            {
                if (url.GetLength() > 0)
                {
                    return GTCore::Strings::Equal(id.c_str(), url.c_str() + 1); // +1 to get past the '#'
                }
            }
            
            return false;
        }
        
        // Determines whether or not a URL is an ID.
        static bool IsURLToID(const GTCore::String &url)
        {
            // The URL references an ID if it's first character is a hash.
            if (url.GetLength() > 0 && url.c_str()[0] == '#')
            {
                return true;
            }
            
            return false;
        }
        
        
        // The asset element, if there is one.
        _asset *asset;
        
        // The cameras library
        _library_cameras *library_cameras;
        
        // The lights library.
        _library_lights *library_lights;
        
        // The geometries library.
        _library_geometries *library_geometries;
        
        // The visual scenes library.
        _library_visual_scenes *library_visual_scenes;
        
        // The scene. Can only be 0 or 1.
        _scene *scene;

    private:    // No copying.
        Collada(const Collada &);
        Collada & operator=(const Collada &);
    };


    void AddCameraComponent(SceneNode *sceneNode, Collada::_camera *camera)
    {
        if (camera)
        {
            // A camera apparently always looks down it's -z axis.
            // TODO: Need to make sure this is correct with X_UP and Y_UP orientations.
            if (camera->asset->upAxis == 2)
            {
                sceneNode->Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            
            // We have a camera.
            if (camera->optics)
            {
                if (camera->optics->perspective)
                {
                    auto cameraComponent = sceneNode->AddComponent<CameraComponent>();
                    
                    float xfov = camera->optics->perspective->xfov;
                    float yfov = camera->optics->perspective->yfov;
                    
                    float fov;
                    if (yfov > 0.0f)
                    {
                        fov = yfov;
                    }
                    else
                    {
                        fov = xfov;
                    }
                    
                    float aspect = camera->optics->perspective->aspect_ratio;
                    float znear  = camera->optics->perspective->znear;
                    float zfar   = camera->optics->perspective->zfar;
                    
                    cameraComponent->Set3DProjection(fov, aspect, znear, zfar);
                }
            }
        }
    }


    void AddLightComponent(SceneNode *sceneNode, Collada::_light *light)
    {
        if (light)
        {
            if (light->point)
            {
                // We have a point light.
                PointLightComponent *lightComponent = sceneNode->AddComponent<PointLightComponent>();
                
                lightComponent->Initialise(glm::vec3(light->point->colourR, light->point->colourG, light->point->colourB),
                                           light->point->constantAttenuation, light->point->linearAttenuation, light->point->quadraticAttenuation);
            }
        }
    }


    void AddGeometryComponent(SceneNode *sceneNode, Collada::_geometry *geometry)
    {
        if (geometry)
        {
            auto modelComponent = sceneNode->AddComponent<ModelComponent>();
            
            // Now we can create a model and attach it to the component.
            auto newModel = new Model;
            
            // Now we just need to loop through each mesh in the geometry node and attach it to the model.
            for (auto i = geometry->meshes.root; i != nullptr; i = i->next)
            {
                newModel->AttachMesh(i->value, nullptr);
            }

            // Here we will generate our tangents and bitangent for every mesh.
            newModel->GenerateTangentsAndBitangents();
            
            // Now lets attach the model to the component.
            modelComponent->SetModel(newModel);
        }
    }


    SceneNode * ToSceneNode(Collada::_node *colladaNode, Collada &collada)
    {
        if (colladaNode)
        {
            auto newNode = new SceneNode;
            newNode->SetName(colladaNode->id.c_str());

            // First is our transforms.
            for (auto i = colladaNode->transforms.root; i != nullptr; i = i->next)
            {
                auto transform = (Collada::_transform *)i->value;
                if (transform)
                {
                    switch (transform->type)
                    {
                        case Collada::transform_type_translate:
                        {
                            auto translate = static_cast<Collada::_translate*>(transform);
                            newNode->Translate(glm::vec3(translate->x, translate->y, translate->z));
                            
                            break;
                        }
                        
                        case Collada::transform_type_rotate:
                        {
                            auto rotate = static_cast<Collada::_rotate*>(transform);
                            newNode->Rotate(rotate->angle, glm::vec3(rotate->x, rotate->y, rotate->z));
                            
                            break;
                        }
                        
                        case Collada::transform_type_scale:
                        {
                            auto scale = static_cast<Collada::_scale*>(transform);
                            newNode->Scale(glm::vec3(scale->x, scale->y, scale->z));
                            
                            break;
                        }

                        case Collada::transform_type_lookat:
                        {
                            auto lookat = static_cast<Collada::_lookat*>(transform);
                            newNode->SetPosition(lookat->px, lookat->py, lookat->pz);
                            newNode->LookAt(glm::vec3(lookat->ix, lookat->iy, lookat->iz), glm::vec3(lookat->upx, lookat->upy, lookat->upz));

                            break;
                        }

                        case Collada::transform_type_none:
                        default: break;
                    }
                }
            }
                    
            // Now we will modify the position, orientation and scale of the node.
            glm::vec3 position = newNode->GetPosition();
            newNode->SetPosition(glm::vec3(colladaNode->asset->position_to_zout(position.x, position.y, position.z, 1.0f)));
            
            glm::quat orientation = newNode->GetOrientation();
            newNode->SetOrientation(colladaNode->asset->orientation_to_zout(orientation));
            
            glm::vec3 scale = newNode->GetScale();
            newNode->SetScale(colladaNode->asset->scale_to_zout(scale.x, scale.y, scale.z));
            
            
            // Cameras.
            if (collada.library_cameras)
            {
                // If we have more than 1 camera we need to create child nodes.
                if (colladaNode->cameras.Count() == 1)
                {
                    AddCameraComponent(newNode, collada.library_cameras->find(((Collada::_instance_camera *)colladaNode->cameras.root->value)->url));
                }
                else if (colladaNode->cameras.Count() > 1)
                {
                    //for (size_t i = 0; i < colladaNode->cameras.size(); ++i)
                    for (auto i = colladaNode->cameras.root; i != nullptr; i = i->next)
                    {
                        auto camera = collada.library_cameras->find(((Collada::_instance_camera *)i->value)->url);
                        if (camera)
                        {
                            auto childNode = new SceneNode;
                            childNode->SetName(camera->name.c_str());

                            AddCameraComponent(childNode, camera);
                            
                            newNode->AttachChild(*childNode);
                        }
                    }
                }
            }
            
            
            // Lights.
            if (collada.library_lights)
            {
                // If we have more than 1 light we need to create child nodes.
                if (colladaNode->lights.Count() == 1)
                {
                    AddLightComponent(newNode, collada.library_lights->find(((Collada::_instance_light *)colladaNode->lights.root->value)->url));
                }
                else if (colladaNode->lights.Count() > 1)
                {
                    //for (size_t i = 0; i < colladaNode->lights.size(); ++i)
                    for (auto i = colladaNode->lights.root; i != nullptr; i = i->next)
                    {
                        auto light = collada.library_lights->find(((Collada::_instance_light *)i->value)->url);
                        if (light)
                        {
                            auto childNode = new SceneNode;
                            childNode->SetName(light->name.c_str());

                            AddLightComponent(childNode, light);
                            
                            newNode->AttachChild(*childNode);
                        }
                    }
                }
            }
            
            
            // Geometries.
            if (collada.library_geometries)
            {
                if (colladaNode->geometries.Count() == 1)
                {
                    AddGeometryComponent(newNode, collada.library_geometries->find(((Collada::_instance_geometry *)colladaNode->geometries.root->value)->url));
                }
                else if (colladaNode->geometries.Count() > 1)
                {
                    for (auto i = colladaNode->geometries.root; i != nullptr; i = i->next)
                    {
                        auto geometry = collada.library_geometries->find(((Collada::_instance_geometry *)i->value)->url);
                        if (geometry)
                        {
                            auto childNode = new SceneNode;
                            childNode->SetName(geometry->name.c_str());

                            AddGeometryComponent(childNode, geometry);
                            
                            newNode->AttachChild(*childNode);
                        }
                    }
                }
            }
            
            
            // Now we need to loop through all of the children and call this function recursively.
            for (auto i = colladaNode->nodes.root; i != nullptr; i = i->next)
            {
                SceneNode *childNode = ToSceneNode((Collada::_node *)i->value, collada);
                if (childNode)
                {
                    newNode->AttachChild(*childNode);
                }
            }
            
            return newNode;
        }
        
        return nullptr;
    }


    SceneNode * BuildSceneNodeFromCollada(Collada &collada)
    {
        SceneNode *sceneNode = nullptr;
        
        // Can't do much without a scene..
        if (collada.scene)
        {
            sceneNode = new SceneNode;
            sceneNode->SetName("ColladaNode");
            
            // If the scene contains a visual scene instance, we can loop through it's nodes and create nodes for those.
            if (collada.scene->visual_scene && collada.library_visual_scenes)
            {
                Collada::_visual_scene *scene = collada.library_visual_scenes->find(collada.scene->visual_scene->url);
                if (scene)
                {
                    // We can now construct SceneNodes from a Collada node.
                    for (auto i = scene->nodes.root; i != nullptr; i = i->next)
                    {
                        SceneNode *newNode = ToSceneNode((Collada::_node *)i->value, collada);
                        if (newNode)
                        {
                            sceneNode->AttachChild(*newNode);
                        }
                    }
                }
            }
        }
        
        return sceneNode;
    }


    SceneNode * ColladaLoader::Parse(const char *content)
    {
        // The string for the parser must be non-constant.
        char *tempContent = (char *)malloc(strlen(content) + 1);
        GTCore::Strings::Copy(tempContent, content);
        
        xml_document<> document;
        document.parse<0>(tempContent);
        
        Collada collada(document.first_node("COLLADA"));
        
        // Now we can try building our scene nodes.
        SceneNode * sceneNode = BuildSceneNodeFromCollada(collada);
        
        // Cant forget to delete the content.
        free(tempContent);
        
        return sceneNode;
    }

    SceneNode * ColladaLoader::Load(const char *fileName)
    {
        FILE *file = GTCore::IO::Open(fileName, GTCore::IO::OpenMode::Read);
        if (file)
        {
            // We need to read the content of the file and then load it as XML. We cast the size to a size_t to
            // play nicely with 32-bit compilations. May run into file size issues with 32-bit platforms in the
            // future, so keep that in mind.
            size_t fileSize = static_cast<size_t>(GTCore::IO::Size(file));
            
            char *fileData = (char *)malloc(fileSize + 1);
            GTCore::IO::Read(file, fileData, fileSize);
            fileData[fileSize] = '\0';
            
            GTCore::IO::Close(file);
            
            SceneNode *result = ColladaLoader::Parse(fileData);
            
            free(fileData);
            return result;
        }
        
        return nullptr;
    }    
}
