/*
 * facade-stub.h — runtime header for the pop3 facade.
 *
 * Pop3Session is pure Amalgame; Pop3Server's TCP/TLS transport uses the
 * runtime's built-in TcpServer/TcpConn plus amalgame-tls. Required only
 * by amc's [stdlib].header; the user binary's #include is a no-op.
 */
