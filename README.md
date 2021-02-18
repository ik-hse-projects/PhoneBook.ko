# Компиляция
1. Отредактировать `vars.sh`
2. `make`

Для отладки также есть `build.sh`, который устанавливает моудль и собирает initramfs.

# Использование

Все команды к модулю завершаются нулевым байтом, в качестве разделителя аргументов используется `\n`.

Далее предполагается, что было создано символьное устройство в `/foo` (например, `mknod /foo c 254 1`), а модуль загружен (`modprobe phonebook`).

## Добавление пользователя
```bash
echo "ADD" > /foo
echo "Surname" > /foo
echo "Name" > /foo
echo "email@example.com" > /foo
echo "+1234567890" > /foo
echo "25" > /foo
echo -ne "\0" > /foo

cat /foo
# Id:0
# Surname: Surname
# Name: Name
# Email: email@example.com
# Phone: +1234567890
# Age: 25
```

## Поиск по фамилии
```bash
echo "FIND" > /foo
echo "Surname" > /foo
echo -ne "\0" > /foo

cat /foo
# Search results:
# Id:0
# Surname: Surname
# Name: Name
# Email: email@example.com
# Phone: +1234567890
# Age: 25
# 
# End
```

## Удаление пользователя
```bash
echo "DEL" > /foo
echo "0" > /foo  # ID пользователя
echo -ne "\0" > /foo

cat /foo
# Deleted 1 matching users
```

