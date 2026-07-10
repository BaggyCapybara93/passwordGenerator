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
| `--custom-chars S` | Custom character pool (e.g., `"abcXYZ123!@#"`) |
| `--exclude-chars S` | Characters to exclude from default pools (e.g., `"!@#$"`) |
| `--no-ambiguous` | Exclude ambiguous characters (0/O, 1/l/I) |
| `--blacklist S` | Comma-separated list of passwords to blacklist (e.g., `{pass1,pass2,pass3}`) |
| `--wordlist-file F` | Path to wordlist file |
| `--blacklist-file F` | Path to blacklist file (default: blacklist.txt) |
| `--min-entropy N` | Set minimum entropy threshold in bits (default: 0 means no minimum) |
| `--honey-password` | Generate a weak password designed to be compromised |
| `--guesses-per-second N` | Set brute-force guesses per second (default: 1e9) |
| `--save-file F` | Path to save generated passwords (default: saved_passwords.txt) |
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

### Exclude ambiguous characters

```bash
./PasswordGenerator --length 16 --no-ambiguous
```

### Blacklist weak/common passwords

```bash
./PasswordGenerator --length 12 --blacklist "{weak123,default,password}"
```

### Use a blacklist file

```bash
./PasswordGenerator --length 16 --blacklist-file "blacklist.txt"
```

### Generate deterministic passwords for testing

```bash
./PasswordGenerator --length 16 --seed 42
```

### Generate passwords with minimum entropy requirement

```bash
./PasswordGenerator --length 24 --min-entropy 80
```

### Generate a weak honey password

```bash
./PasswordGenerator --honey-password
```

### Customize guesses per second for entropy calculations

```bash
./PasswordGenerator --length 24 --min-entropy 80 --guesses-per-second 1e8
```

### Save passwords to a file

```bash
./PasswordGenerator --length 16 --num-passwords 10 --save-file "my_passwords.txt"
```

---

## Minimum Entropy Feature

The minimum entropy feature ensures that generated passwords meet a specified entropy threshold. If a generated password's entropy is below the minimum threshold, it will be automatically regenerated until it meets the requirement.

### What is Entropy?

Entropy measures the unpredictability or randomness of a password. Higher entropy means more secure passwords. Entropy is calculated in bits and depends on:
- The size of the character pool used
- The length of the password

### Usage

```bash
./PasswordGenerator --min-entropy N
```

Where `N` is the minimum entropy threshold in bits.

### Examples

```bash
./PasswordGenerator --min-entropy 60
./PasswordGenerator --length 24 --min-entropy 80
./PasswordGenerator --length 32 --no-special --min-entropy 70
```

### Security Guidelines

- **40-60 bits**: Moderate security - suitable for low-risk applications
- **60-80 bits**: Good security - recommended for most applications
- **80+ bits**: Strong security - recommended for high-security applications
- **128+ bits**: Maximum security - recommended for cryptographic keys

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