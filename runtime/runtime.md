# lamina ffi 
lamina ffi 使用了第三方库[dyncall](https://dyncall.org/), 
相关证书参见[LICENSE](dyncall-1.4/LICENSE), lamina-dev保留dyncall版权声明：
#### dyncall 
Copyright (c) 2007-2022 Daniel Adler \<dadler AT uni-goettingen DOT de>,
Tassilo Philipp \<tphilipp AT potion-studios DOT com>

### VMCALL api
1. stdout
    arg1: str(reg)
2. stdin
    arg1: strbuf(reg)
3. exit
    arg1: code(reg)
4. ...