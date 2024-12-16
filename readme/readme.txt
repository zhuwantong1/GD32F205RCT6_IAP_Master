AA AA AA AA AA  -> app1主控板的更新指令，将程序bin1烧写到flash1
BB BB BB BB BB  -> app2驱动板的更新指令，将程序bin2烧写到flash2
AA AA AA AA 11  -> 直接跳转到APP1，前提是已经烧写了bin1
BB BB BB BB 11  -> 发送指令给驱动板，并读出存放在flash2区域的bin2，发送bin2，这里只管发，接收是驱动板要做的事情




