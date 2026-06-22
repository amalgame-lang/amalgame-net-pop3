# amalgame-net-pop3

A **POP3 server** (RFC 1939 + STLS, RFC 2595) for the native Amalgame
mail server (Phase 5). The legacy download-and-go access protocol: a
client logs in, downloads messages, optionally deletes them. Reads an
[`amalgame-net-mail-store`](https://github.com/amalgame-lang/amalgame-net-mail-store)
MailStore. Most modern clients use
[`amalgame-net-imap`](https://github.com/amalgame-lang/amalgame-net-imap)
instead; POP3 is here for legacy compatibility. See
[`native-mail-server.md`](https://github.com/amalgame-lang/Amalgame/blob/main/docs/proposals/native-mail-server.md).

## Two layers

| Layer | Status |
|---|---|
| **`Pop3Session`** — the POP3 command state machine (pure logic, socket/TLS-free, unit-tested 12/12) | ✅ v0.1.0 |
| **`Pop3Server`** — the TcpServer accept loop + STLS upgrade (transport, loopback smoke-tested) | ✅ v0.1.0 |

```amalgame
import Amalgame.Net.Pop3
import Amalgame.Net.Mail

let store: MailStore = MailStore.Open("/var/mail/alice")
let srv:   Pop3Server = new Pop3Server(store, "mail.example.com")
srv.AddPassword("alice", "s3cret")                 // or AddUserHash(name, scryptHash)
let s2: Pop3Server = srv.WithCert("/etc/ssl/cert.pem", "/etc/ssl/key.pem")
s2.Serve(110)                                       // blocking; one connection at a time (v0.1)
```

## Command coverage (v0.1.0)

- **CAPA** — advertises `STLS`, `USER`, `UIDL`, `TOP`.
- **STLS** — the transport upgrades the socket via `amalgame-tls`.
- **USER / PASS** — verified against **scrypt** hashes via
  `amalgame-crypto` `Password.Verify` (the same hashes `amalgame-auth`
  stores → one unified account). Refused before STLS (no cleartext
  passwords).
- **STAT** — message count + total size (excluding deleted).
- **LIST / UIDL** — scan listing / unique-id (the store UID) listing,
  single-message or multi-line.
- **RETR** — full message, **dot-stuffed**, terminated with CRLF `.` CRLF.
- **TOP** — headers + the first *n* body lines.
- **DELE / RSET** — mark / unmark for deletion (applied at QUIT).
- **NOOP / QUIT** — QUIT enters the UPDATE state and applies the marked
  deletions to the store.

Verified end-to-end by `tests/smoke_test.sh`: a real Python `poplib`
client does STLS → USER/PASS → STAT → RETR and reads the message back.

## Out of scope (v0.1.0)

- `APOP`; a worker pool (v0.1 serves one connection at a time);
  implicit TLS on `:995`.

## Dependencies

- [`amalgame-net-mail-store`](https://github.com/amalgame-lang/amalgame-net-mail-store) `>=0.1.0`
- [`amalgame-crypto`](https://github.com/amalgame-lang/amalgame-crypto) `>=0.6.0`
- [`amalgame-tls`](https://github.com/amalgame-lang/amalgame-tls) `>=0.3.5`

## Tests

```sh
# siblings net-mail-store, io-filesystem, database-sqlite, crypto, tls alongside
bash tests/run_tests.sh  /path/to/amc     # Pop3Session unit tests (12/12)
bash tests/smoke_test.sh /path/to/amc     # loopback POP3+STLS via Python poplib
```

## License

Apache-2.0 — see `LICENSE` and `NOTICE.md`.
