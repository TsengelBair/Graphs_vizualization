#!/bin/bash

# Текущая директория
project_dir="."

# Поиск и обработка всех файлов .cpp и .h
find "$project_dir" -type f \( -name "*.cpp" -o -name "*.h" \) | while read -r file; do
    echo "Processing file: $file"

    # Замена комментариев в стиле // на комментарии в стиле /* */
    sed -i -E 's|//(.*)|/*\1 */|g' "$file"

    # Проверка, что команда sed выполнена успешно
    if [ $? -eq 0 ]; then
        echo "Formatted comments in $file successfully."
    else
        echo "Error formatting comments in $file"
    fi
done
