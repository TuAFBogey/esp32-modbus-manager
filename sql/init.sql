-- Initialize database schema
-- Clean existing tables if any
DROP TABLE IF EXISTS RegisterHistory;
DROP TABLE IF EXISTS ModbusRegister;
DROP TABLE IF EXISTS ModbusDevice;
DROP TABLE IF EXISTS AuthToken;
DROP TABLE IF EXISTS User;

-- Create User table
CREATE TABLE User (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    username    TEXT UNIQUE NOT NULL,
    password    TEXT NOT NULL,
    created_at  INTEGER DEFAULT (strftime('%s','now'))
);

-- Create AuthToken table
CREATE TABLE AuthToken (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id     INTEGER NOT NULL,
    token       TEXT UNIQUE NOT NULL,
    expires_at  INTEGER NOT NULL,
    created_at  INTEGER DEFAULT (strftime('%s','now')),
    FOREIGN KEY(user_id) REFERENCES User(id)
);

-- Create ModbusDevice table
CREATE TABLE ModbusDevice (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id   TEXT UNIQUE NOT NULL,
    slave_id    INTEGER NOT NULL CHECK(slave_id BETWEEN 1 AND 247),
    ip_address  TEXT DEFAULT '0.0.0.0',
    port        INTEGER DEFAULT 502,
    device_name TEXT NOT NULL,
    model       TEXT,
    last_seen   INTEGER DEFAULT (strftime('%s','now')),
    status      TEXT DEFAULT 'OFFLINE',
    created_at  INTEGER DEFAULT (strftime('%s','now'))
);

-- Create ModbusRegister table
CREATE TABLE ModbusRegister (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id       INTEGER NOT NULL,
    register_type   TEXT NOT NULL CHECK(register_type IN ('COIL', 'DISCRETE_INPUT', 'INPUT', 'HOLDING')),
    address         INTEGER NOT NULL,
    value           INTEGER DEFAULT 0,
    min_value       INTEGER DEFAULT 0,
    max_value       INTEGER DEFAULT 100,
    default_value   INTEGER DEFAULT 0,
    read_only       INTEGER DEFAULT 0,
    description     TEXT,
    updated_at      INTEGER DEFAULT (strftime('%s','now')),
    created_at      INTEGER DEFAULT (strftime('%s','now')),
    FOREIGN KEY(device_id) REFERENCES ModbusDevice(id),
    UNIQUE(device_id, register_type, address)
);

-- Create RegisterHistory table
CREATE TABLE RegisterHistory (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    register_id INTEGER NOT NULL,
    user_id     INTEGER NOT NULL,
    old_value   INTEGER,
    new_value   INTEGER,
    action      TEXT NOT NULL CHECK(action IN ('READ', 'WRITE')),
    timestamp   INTEGER DEFAULT (strftime('%s','now')),
    FOREIGN KEY(register_id) REFERENCES ModbusRegister(id),
    FOREIGN KEY(user_id) REFERENCES User(id)
);

-- Insert default admin user
INSERT INTO User (username, password) 
VALUES ('admin', '9c0932d8b8e8cf401ae0f4c53826a446');

-- Create indexes for better performance
CREATE INDEX idx_authtoken_token ON AuthToken(token);
CREATE INDEX idx_authtoken_user ON AuthToken(user_id);
CREATE INDEX idx_register_device ON ModbusRegister(device_id);
CREATE INDEX idx_register_address ON ModbusRegister(address);
CREATE INDEX idx_history_register ON RegisterHistory(register_id);
CREATE INDEX idx_history_user ON RegisterHistory(user_id);
CREATE INDEX idx_device_slave ON ModbusDevice(slave_id);