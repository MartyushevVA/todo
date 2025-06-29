-- Создание таблицы tasks
CREATE TABLE IF NOT EXISTS tasks (
    id SERIAL PRIMARY KEY,
    author TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    title TEXT UNIQUE,
    content TEXT NOT NULL,
    completed BOOLEAN
);

-- Создание таблицы users
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    login TEXT UNIQUE NOT NULL,
    passwd TEXT NOT NULL
);

-- Добавляем тестового пользователя
INSERT INTO users (login, passwd) VALUES ('lemotip', '123');
