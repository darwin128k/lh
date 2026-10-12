/**
 * @file ws.cpp
 * @brief WebSocket frames: ping/pong, close, mask, size, continuation.
 */

#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/web/ws.h>
#include <lh/util/addr.h>

#include <cstring>
#include <string>
#include <vector>

TEST(web_ws, text_roundtrip_unmasked)
{
    unsigned char buf[64];
    const char *hi = "hi";
    const lh_usize_t n =
        lh_ui_web_ws_write(buf, sizeof(buf), LH_UI_WEB_WS_TEXT, lh_bool_true, hi, 2u);
    unsigned opcode = 99;
    lh_bool_t fin = lh_bool_false;
    const unsigned char *payload = lh_null;
    lh_usize_t plen = 0;
    const unsigned char *mask = (const unsigned char *)1;

    ASSERT_EQ(n, 4u);
    ASSERT_EQ(lh_ui_web_ws_read(buf, n, lh_addr_of(opcode), lh_addr_of(fin), lh_addr_of(payload),
                                lh_addr_of(plen), lh_addr_of(mask)),
              n);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_TEXT);
    EXPECT_EQ(fin, lh_bool_true);
    EXPECT_EQ(plen, 2u);
    EXPECT_EQ(mask, static_cast<const unsigned char *>(lh_null));
    EXPECT_EQ(std::string(reinterpret_cast<const char *>(payload), 2), "hi");
}

TEST(web_ws, masked_client_frame_unmasks)
{
    unsigned char frame[] = {0x81, 0x83, 0x01, 0x02, 0x03, 0x04, (unsigned char)('a' ^ 1),
                             (unsigned char)('b' ^ 2), (unsigned char)('c' ^ 3)};
    unsigned opcode = 0;
    lh_bool_t fin = lh_bool_false;
    const unsigned char *payload = lh_null;
    lh_usize_t plen = 0;
    const unsigned char *mask = lh_null;
    unsigned char copy[3];

    ASSERT_EQ(lh_ui_web_ws_read(frame, sizeof(frame), lh_addr_of(opcode), lh_addr_of(fin),
                                lh_addr_of(payload), lh_addr_of(plen), lh_addr_of(mask)),
              sizeof(frame));
    ASSERT_EQ(plen, 3u);
    ASSERT_NE(mask, static_cast<const unsigned char *>(lh_null));
    std::memcpy(copy, payload, 3);
    lh_ui_web_ws_unmask(copy, 3, mask);
    EXPECT_EQ(copy[0], 'a');
    EXPECT_EQ(copy[1], 'b');
    EXPECT_EQ(copy[2], 'c');
}

TEST(web_ws, length_126_and_prefix_is_zero)
{
    std::vector<unsigned char> payload(200, 'x');
    std::vector<unsigned char> buf(lh_ui_web_ws_frame_size(payload.size()));
    const lh_usize_t n = lh_ui_web_ws_write(buf.data(), buf.size(), LH_UI_WEB_WS_BIN, lh_bool_true,
                                            payload.data(), payload.size());
    unsigned opcode = 0;
    lh_usize_t plen = 0;
    const unsigned char *body = lh_null;

    ASSERT_EQ(n, 4u + 200u);
    EXPECT_EQ(lh_ui_web_ws_read(buf.data(), 3u, lh_null, lh_null, lh_null, lh_null, lh_null), 0u);
    ASSERT_EQ(lh_ui_web_ws_read(buf.data(), n, lh_addr_of(opcode), lh_null, lh_addr_of(body),
                                lh_addr_of(plen), lh_null),
              n);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_BIN);
    EXPECT_EQ(plen, 200u);
}

TEST(web_ws, length_127_rejects_over_max)
{
    unsigned char hdr[10] = {0x81, 127, 0, 0, 0, 0, 0, 0x20, 0, 0};

    EXPECT_EQ(lh_ui_web_ws_read(hdr, sizeof(hdr), lh_null, lh_null, lh_null, lh_null, lh_null),
              (lh_usize_t)-1);
}

TEST(web_ws, ping_pong_close_opcodes)
{
    unsigned char buf[16];
    unsigned opcode = 0;
    const char *pay = "xy";

    ASSERT_GT(lh_ui_web_ws_write(buf, sizeof(buf), LH_UI_WEB_WS_PING, lh_bool_true, pay, 2u), 0u);
    ASSERT_EQ(lh_ui_web_ws_read(buf, 16u, lh_addr_of(opcode), lh_null, lh_null, lh_null, lh_null) > 0u,
              true);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_PING);
    ASSERT_GT(lh_ui_web_ws_write(buf, sizeof(buf), LH_UI_WEB_WS_PONG, lh_bool_true, pay, 2u), 0u);
    ASSERT_EQ(lh_ui_web_ws_read(buf, 16u, lh_addr_of(opcode), lh_null, lh_null, lh_null, lh_null) > 0u,
              true);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_PONG);
    ASSERT_GT(lh_ui_web_ws_write(buf, sizeof(buf), LH_UI_WEB_WS_CLOSE, lh_bool_true, lh_null, 0u), 0u);
    ASSERT_EQ(lh_ui_web_ws_read(buf, 16u, lh_addr_of(opcode), lh_null, lh_null, lh_null, lh_null) > 0u,
              true);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_CLOSE);
}

TEST(web_ws, length_127_roundtrip_over_64k)
{
    std::vector<unsigned char> payload(70000, 'z');
    std::vector<unsigned char> buf(lh_ui_web_ws_frame_size(payload.size()));
    const lh_usize_t n = lh_ui_web_ws_write(buf.data(), buf.size(), LH_UI_WEB_WS_BIN, lh_bool_true,
                                            payload.data(), payload.size());
    unsigned opcode = 0;
    lh_usize_t plen = 0;
    const unsigned char *body = lh_null;

    ASSERT_EQ(n, 10u + 70000u);
    ASSERT_EQ(lh_ui_web_ws_read(buf.data(), n, lh_addr_of(opcode), lh_null, lh_addr_of(body),
                                lh_addr_of(plen), lh_null),
              n);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_BIN);
    EXPECT_EQ(plen, 70000u);
    EXPECT_EQ(body[0], 'z');
    EXPECT_EQ(body[69999], 'z');
}

TEST(web_ws, bad_opcode_is_error)
{
    unsigned char bad[] = {0x83, 0x00};

    EXPECT_EQ(lh_ui_web_ws_read(bad, sizeof(bad), lh_null, lh_null, lh_null, lh_null, lh_null),
              (lh_usize_t)-1);
}

TEST(web_ws, continuation_second_frame_has_cont_opcode)
{
    unsigned char a[16];
    unsigned char b[16];
    unsigned opcode = 0;
    lh_bool_t fin = lh_bool_true;

    ASSERT_GT(lh_ui_web_ws_write(a, sizeof(a), LH_UI_WEB_WS_TEXT, lh_bool_false, "ab", 2u), 0u);
    ASSERT_GT(lh_ui_web_ws_write(b, sizeof(b), LH_UI_WEB_WS_CONT, lh_bool_true, "cd", 2u), 0u);
    ASSERT_EQ(lh_ui_web_ws_read(a, sizeof(a), lh_addr_of(opcode), lh_addr_of(fin), lh_null, lh_null,
                                lh_null) > 0u,
              true);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_TEXT);
    EXPECT_EQ(fin, lh_bool_false);
    ASSERT_EQ(lh_ui_web_ws_read(b, sizeof(b), lh_addr_of(opcode), lh_addr_of(fin), lh_null, lh_null,
                                lh_null) > 0u,
              true);
    EXPECT_EQ(opcode, LH_UI_WEB_WS_CONT);
    EXPECT_EQ(fin, lh_bool_true);
}
