# Используем официальный образ PostgreSQL как базовый
FROM postgres:15

# Устанавливаем переменные окружения для базы
ENV POSTGRES_USER=lemotip
ENV POSTGRES_PASSWORD=wsxmok
ENV POSTGRES_DB=tasks

# Копируем скрипты инициализации (если есть) в специальную папку
COPY ./init.sql /docker-entrypoint-initdb.d/

# Порт по умолчанию
EXPOSE 5432
