# Password Generator

A simple C++ password generator

---

## Usage

```bash
./PasswordGenerator [options]
```

### Options

| Option | Description |
|--------|-------------|
| `--length N` | Set desired password length (default: 12) |
| `--no-uppercase` | Disable uppercase requirement |
| `--no-lowercase` | Disable lowercase requirement |
| `--no-digits` | Disable digit requirement |
| `--no-special` | Disable special character requirement |
| `--no-color` | Disable colored output |
| `--num-passwords N` | Number of passwords to generate (default: 1) |
| `--seed N` | Use deterministic seed for random generation |
| `--blacklist S` | Comma-separated list of passwords to blacklist (e.g., `{pass1,pass2,pass3}`) |
| `--help`, `-h` | Show this help message and exit |

---

## Examples

### Generate multiple passwords without special characters

```bash
./PasswordGenerator --length 32 --no-special --num-passwords 5
```

### Use a custom character pool

```bash
./PasswordGenerator --length 16 --custom-chars "abcXYZ123!@#"
```

### Exclude specific characters from default pools

```bash
./PasswordGenerator --length 16 --exclude-chars "!@#$"
```

### Blacklist weak/common passwords

```bash
./PasswordGenerator --length 12 --blacklist "{weak123,default,password}"
```

### Generate deterministic passwords for testing

```bash
./PasswordGenerator --length 16 --seed 42
```

---

## Blacklist Feature

The blacklist feature allows you to block specific passwords from being generated. If a generated password matches a blacklisted password, it will be automatically regenerated until a non-blacklisted password is produced.

### Format

```
{password1,password2,password3}
```

### Examples

```bash
./PasswordGenerator --blacklist "{weak123,default,password}"
./PasswordGenerator --length 16 --blacklist "{admin123,root,password123}"
```
---

## Building

```bash
mkdir build && cd build
cmake ..
make
```