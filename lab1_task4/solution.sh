# 1. Создать каталог out, если его нет, иначе удалить содержимое и сам каталог
wsl
test -d out && rm -rf out
mkdir out

# 2. Перейти внутрь каталога out
cd out

# 3. Создать файл me.txt с именем текущего пользователя
whoami > me.txt

# 4. Скопировать содержимое me.txt в файл metoo.txt
cp me.txt metoo.txt

# 5. Создать файл wchelp.txt с документацией по команде wc
man wc > wchelp.txt

# 6. Вывести содержимое файла wchelp.txt в консоль
cat wchelp.txt

# 7. Записать количество строк файла wchelp.txt в файл wchelp-lines.txt
wc -l wchelp.txt | cut -d ' ' -f1 > wchelp-lines.txt

# 8. Создать файл wchelp-reversed.txt с содержимым wchelp.txt в обратном порядке
tac wchelp.txt > wchelp-reversed.txt

# 9. Объединить файлы в all.txt
cat wchelp.txt wchelp-reversed.txt me.txt metoo.txt wchelp-lines.txt > all.txt

# 10. Создать архив result.tar с файлами .txt
tar -cf result.tar *.txt

# 11. Сжать архив result.tar в файл result.tar.gz
gzip result.tar

# 12. Выйти из каталога out в родительский каталог
cd ..

# 13. Переместить result.tar.gz из out в текущий каталог
test -f result.tar.gz && rm result.tar.gz
mv out/result.tar.gz .

# 14. Удалить каталог out
rm -rf out

#насроить права через chmod