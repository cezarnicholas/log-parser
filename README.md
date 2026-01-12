basic log parser i made to accomodate myself with modern c++ stl features, will prob add more stuff to this when i have the time

log format:
TIMESTAMP | LEVEL | SERVICE | PID | MESSAGE | METADATA

example log:
2026-01-11T19:41:27.100Z | DEBUG | api-gateway    | 3099 | Request received | method=POST,path=/api/v1/orders,status=201

the parser assumes this format (especially the metadata being comma-separated with no spaces) is always respected
feel free to modify or use for learning purposes
