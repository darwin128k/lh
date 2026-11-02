# HTTP/1 message-head parser

`<lh/http.h>` is an allocation-free protocol brick. It parses request/status
lines and fields from byte views. It does not open sockets, read files, manage
threads, decode a body or decide which methods a service allows.

## Example

```c
#include <lh/http.h>

const char input[] = "GET /maps/example.bsp HTTP/1.1\r\nHost: example\r\n\r\n";
lh_str_view_t bytes = lh_str_view_initializer(input, sizeof input - 1);
lh_http_parser_t parser;
lh_http_request_t request;
lh_http_header_t headers[16];
lh_http_parser_init(&parser);
lh_http_parse_status_t status = lh_http_parser_request(&parser, bytes, &request, headers, 16);
/* On complete: request.method/target, headers[0..header_count), get_consumed(). */
```

## Requests and responses

For a response use `lh_http_parser_response` and `lh_http_response_t`.
Both interfaces return complete/incomplete/error. Empty input is incomplete.
On incomplete, append bytes to the input and retry from its beginning. The
parser does not retain the input or an incremental cursor between calls.

## Lifetime of the parsed views

Only inspect parsed output on complete. All views point into the caller's
original buffer and expire if it moves, changes or is freed. The header array
also belongs to the caller. Names retain their spelling; duplicate fields
stay separate. Consumers must compare field names without ASCII case and
apply their own duplicate/framing rules.

## Framing: what the parser does not decide

`get_consumed` gives the byte offset immediately after the terminating empty
line. It does not include a body or a pipelined message. A file service can
require this to equal the received byte count and reject Content-Length and
Transfer-Encoding; a general HTTP client must implement body framing itself.
Do not treat successful head parsing as a full HTTP validation policy.

## Syntax

Syntax follows the message-head rules of
[RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html) with strict CRLF and
rejection of obsolete folding. Targets remain raw ASCII URI text; percent
decoding and path validation are separate operations. Values preserve
non-ASCII octets without interpreting their encoding. HTTP/1 minor versions
are single digits; supported protocol versions remain application policy.

## Limits

Default budgets are 8192 head bytes including CRLF and 4096 line-text bytes
excluding CRLF. Use `lh_http_parser_set_limits` for other nonzero budgets.
Storage capacity independently limits field count. Errors report a category
and input offset. Header bytes already inspected before incomplete/error may
have populated storage; those partial values are not a successful result.

## Bricks

The two parse calls only compose public steps, each usable and tested on its
own:

| Brick | Does |
|-------|------|
| `lh_http_parser_begin` | clears diagnostics, checks arguments and budgets |
| `lh_http_parser_find_line` | finds the CRLF of one physical line within the budgets |
| `lh_http_parser_parse_request_line` / `_status_line` | the start line |
| `lh_http_parser_parse_field` | one `name: value` line |
| `lh_http_parser_parse_fields` | field lines up to the empty line, records `consumed` |
| `lh_http_parser_fail` | the one place an error is recorded |
| `lh_http_parse_version` (`lh/http/version.h`) | the `HTTP/1.x` token |
| `lh_http_is_token_char`, `_is_target_char`, `_is_field_char` (`lh/http/char.h`) | byte classes |

Sub-views come from `lh_str_view_from_offset`, ASCII classes from `lh/char`,
value trimming from `lh_str_view_trim`. The parser allocates nothing, keeps no
global state and makes no OS call; it is part of the normal `lh` library.

## Tests

`lh_test_http_parser` (GoogleTest, common runner) covers the parse calls and
every brick above.
