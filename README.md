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
| `--quiet` | Print only generated passwords to standard output |
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
| `--save-file F` | Create a private file for generated passwords; fails if the file already exists |
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

The file is created with access limited to the current user. Existing files, including symbolic links, are never overwritten. Choose a new path for each run.

### Pipe passwords into another command

```bash
./PasswordGenerator --quiet --num-passwords 5 | tee passwords.txt
```

In quiet mode, stdout contains one password per line. Errors are written to stderr.

---

## Minimum Entropy Feature

The minimum entropy feature checks the number of valid passwords available under the selected settings before generation. Passwords are sampled uniformly from that valid set, and generation stops with an error if the available set is below the requested threshold.

### What is Entropy?

The reported search-space entropy is `log2(number of valid passwords)`. It accounts for password length, the filtered character pool, enabled character requirements, and blacklisted passwords. It describes the generator's output space; it does not estimate the strength of a password chosen by a person or model real-world attack conditions. To keep generation bounded, the program reports an error if it cannot find an allowed password within one million attempts.

Honey passwords are intentionally weak, so the program labels them as such and does not display a character-pool entropy estimate.

### Usage

```bash
./PasswordGenerator --min-entropy N
```

Where `N` is the minimum search-space entropy threshold in bits.

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

## Randomness

By default, generated passwords use the operating system's cryptographic random-number generator (`/dev/urandom` on Unix-like systems and `BCryptGenRandom` on Windows). The `--seed` option intentionally switches to deterministic generation for testing and should not be used for real passwords.

---

## Blacklist Feature

The blacklist feature removes specific passwords from the output space. Generation continues until it samples a password that is not blacklisted.

For `--custom-chars`, the provided characters form the allowed pool. Enabled uppercase, lowercase, digit, and special requirements mean the output must contain at least one character from each corresponding group. Special characters are printable ASCII punctuation.

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
