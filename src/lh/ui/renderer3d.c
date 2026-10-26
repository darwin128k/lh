#include <lh/ui/renderer3d.h>
#include <math.h>
#include <float.h>
#include <string.h>
typedef struct
{
    float x, y, z;
} vertex;
static vertex
vec(lh_math_vec3_t a)
{
    return (vertex){lh_math_vec3_get_x(&a), lh_math_vec3_get_y(&a), lh_math_vec3_get_z(&a)};
}
static lh_math_vec3_t
out(vertex a)
{
    lh_math_vec3_t v;
    lh_math_vec3_init(&v, a.x, a.y, a.z);
    return v;
}
static float
dot(vertex a, vertex b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static vertex
cross(vertex a, vertex b)
{
    return (vertex){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
static int
finite3(vertex a)
{
    return isfinite(a.x) && isfinite(a.y) && isfinite(a.z);
}
static int
normalize(vertex *a)
{
    float n = sqrtf(dot(*a, *a));
    if (!isfinite(n) || n < 1e-6f)
    {
        return 0;
    }
    a->x /= n;
    a->y /= n;
    a->z /= n;
    return 1;
}
lh_bool_t
lh_ui_renderer3d_camera_init(lh_ui_renderer3d_camera_t *s, lh_math_vec3_t pos, lh_math_vec3_t f,
                             lh_math_vec3_t u, float fov, float near_z, float far_z)
{
    if (!s || !finite3(vec(pos)) || !finite3(vec(f)) || !finite3(vec(u)) || !isfinite(fov) ||
        fov < .1f || fov > 3.f || !isfinite(near_z) || !isfinite(far_z) || near_z <= 0 ||
        far_z <= near_z)
    {
        return 0;
    }
    vertex forward = vec(f), up = vec(u);
    if (!normalize(&forward))
    {
        return 0;
    }
    vertex right = cross(forward, up);
    if (!normalize(&right))
    {
        return 0;
    }
    up = cross(right, forward);
    s->position = pos;
    s->forward = out(forward);
    s->right = out(right);
    s->up = out(up);
    s->tangent = tanf(fov * .5f);
    s->near_plane = near_z;
    s->far_plane = far_z;
    return 1;
}
lh_bool_t
lh_ui_renderer3d_begin(lh_ui_renderer3d_t *s, lh_ui_pixmap_t *target, float *depth,
                       lh_usize_t count, const lh_ui_renderer3d_camera_t *cam, lh_u32_t bg)
{
    if (!s)
    {
        return 0;
    }
    memset(s, 0, sizeof *s);
    if (!target || !depth || !cam ||
        lh_ui_pixmap_get_format(target) != lh_ui_pixmap_format_argb8888)
    {
        return 0;
    }
    int w = lh_ui_pixmap_get_width(target), h = lh_ui_pixmap_get_height(target);
    if (w < 1 || h < 1 || w > 8192 || h > 8192 || (lh_usize_t)w * (lh_usize_t)h > count ||
        !isfinite(cam->tangent) || cam->tangent <= 0 || cam->near_plane <= 0 ||
        cam->far_plane <= cam->near_plane)
    {
        return 0;
    }
    s->target = target;
    s->depth = depth;
    s->width = w;
    s->height = h;
    s->camera = *cam;
    for (int y = 0; y < h; y++)
    {
        lh_u32_t *row = (lh_u32_t *)lh_ui_pixmap_get_row(target, y);
        if (!row)
        {
            s->target = 0;
            return 0;
        }
        for (int x = 0; x < w; x++)
        {
            row[x] = bg;
            depth[(lh_usize_t)y * w + x] = FLT_MAX;
        }
    }
    return 1;
}
static vertex
transform(lh_ui_renderer3d_t *s, lh_math_vec3_t v)
{
    vertex p = vec(v), o = vec(s->camera.position);
    p.x -= o.x;
    p.y -= o.y;
    p.z -= o.z;
    return (vertex){dot(p, vec(s->camera.right)), dot(p, vec(s->camera.up)),
                    dot(p, vec(s->camera.forward))};
}
static float
distance_plane(vertex v, int plane, float tx, float ty, float near_z, float far_z)
{
    switch (plane)
    {
    case 0:
        return v.z - near_z;
    case 1:
        return far_z - v.z;
    case 2:
        return v.x + v.z * tx;
    case 3:
        return v.z * tx - v.x;
    case 4:
        return v.y + v.z * ty;
    default:
        return v.z * ty - v.y;
    }
}
static float
edge(vertex a, vertex b, float x, float y)
{
    return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}
static void
raster(lh_ui_renderer3d_t *s, vertex a, vertex b, vertex c, lh_u32_t color)
{
    vertex p[3] = {a, b, c};
    float f = s->height / (2 * s->camera.tangent);
    for (int i = 0; i < 3; i++)
    {
        p[i].x = s->width * .5f + p[i].x * f / p[i].z;
        p[i].y = s->height * .5f - p[i].y * f / p[i].z;
        p[i].z = 1 / p[i].z;
    }
    a = p[0];
    b = p[1];
    c = p[2];
    float area = edge(a, b, c.x, c.y);
    if (!isfinite(area) || fabsf(area) < 1e-5f)
    {
        return;
    }
    int x0 = (int)fmaxf(0, floorf(fminf(a.x, fminf(b.x, c.x)))),
        x1 = (int)fminf(s->width - 1, ceilf(fmaxf(a.x, fmaxf(b.x, c.x))));
    int y0 = (int)fmaxf(0, floorf(fminf(a.y, fminf(b.y, c.y)))),
        y1 = (int)fminf(s->height - 1, ceilf(fmaxf(a.y, fmaxf(b.y, c.y))));
    s->triangles++;
    for (int y = y0; y <= y1; y++)
    {
        lh_u32_t *row = (lh_u32_t *)lh_ui_pixmap_get_row(s->target, y);
        for (int x = x0; x <= x1; x++)
        {
            float wa = edge(b, c, x + .5f, y + .5f) / area,
                  wb = edge(c, a, x + .5f, y + .5f) / area, wc = 1 - wa - wb;
            if (wa < -.00001f || wb < -.00001f || wc < -.00001f)
            {
                continue;
            }
            float iz = wa * a.z + wb * b.z + wc * c.z;
            if (iz <= 0)
            {
                continue;
            }
            float z = 1 / iz;
            lh_usize_t i = (lh_usize_t)y * s->width + x;
            if (z < s->depth[i])
            {
                s->depth[i] = z;
                row[x] = color;
            }
        }
    }
}
lh_void
lh_ui_renderer3d_triangle(lh_ui_renderer3d_t *s, lh_math_vec3_t a, lh_math_vec3_t b,
                          lh_math_vec3_t c, lh_u32_t color)
{
    if (!s || !s->target || !finite3(vec(a)) || !finite3(vec(b)) || !finite3(vec(c)))
    {
        return;
    }
    vertex buf[2][16];
    buf[0][0] = transform(s, a);
    buf[0][1] = transform(s, b);
    buf[0][2] = transform(s, c);
    int n = 3, src = 0;
    float ty = s->camera.tangent, tx = ty * s->width / s->height;
    for (int plane = 0; plane < 6 && n; plane++)
    {
        int m = 0;
        vertex prev = buf[src][n - 1];
        float dp = distance_plane(prev, plane, tx, ty, s->camera.near_plane, s->camera.far_plane);
        for (int i = 0; i < n; i++)
        {
            vertex cur = buf[src][i];
            float dc =
                distance_plane(cur, plane, tx, ty, s->camera.near_plane, s->camera.far_plane);
            if ((dp >= 0) != (dc >= 0))
            {
                float t = dp / (dp - dc);
                if (m >= 16)
                {
                    return;
                }
                buf[1 - src][m++] =
                    (vertex){prev.x + (cur.x - prev.x) * t, prev.y + (cur.y - prev.y) * t,
                             prev.z + (cur.z - prev.z) * t};
            }
            if (dc >= 0)
            {
                if (m >= 16)
                {
                    return;
                }
                buf[1 - src][m++] = cur;
            }
            prev = cur;
            dp = dc;
        }
        n = m;
        src = 1 - src;
    }
    for (int i = 1; i + 1 < n; i++)
    {
        raster(s, buf[src][0], buf[src][i], buf[src][i + 1], color);
    }
}
lh_void
lh_ui_renderer3d_box(lh_ui_renderer3d_t *s, lh_math_vec3_t minimum, lh_math_vec3_t maximum,
                     lh_u32_t color)
{
    vertex a = vec(minimum), b = vec(maximum);
    if (!finite3(a) || !finite3(b) || a.x > b.x || a.y > b.y || a.z > b.z)
    {
        return;
    }
    lh_math_vec3_t v[8];
    for (int i = 0; i < 8; i++)
    {
        v[i] = out((vertex){i & 1 ? b.x : a.x, i & 2 ? b.y : a.y, i & 4 ? b.z : a.z});
    }
    static const unsigned char faces[12][3] = {{0, 1, 3}, {0, 3, 2}, {4, 6, 7}, {4, 7, 5},
                                               {0, 4, 5}, {0, 5, 1}, {2, 3, 7}, {2, 7, 6},
                                               {0, 2, 6}, {0, 6, 4}, {1, 5, 7}, {1, 7, 3}};
    for (int i = 0; i < 12; i++)
    {
        lh_ui_renderer3d_triangle(s, v[faces[i][0]], v[faces[i][1]], v[faces[i][2]], color);
    }
}
lh_u32_t
lh_ui_renderer3d_get_triangles(const lh_ui_renderer3d_t *s)
{
    return s ? s->triangles : 0;
}
