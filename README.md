# Custom Wokwi Parts Collection

This repository contains a collection of custom parts for use in Wokwi projects. These parts are designed to extend the functionality of Wokwi, allowing for more complex and realistic simulations.

## Available Parts

1. ATECC608 Crypto Authentication Chip
   - Simulates core functionalities of the ATECC608 secure element
   - Supports cryptographic operations and secure storage

(List other parts here as they are added to the collection)

## General Installation

To use any of these custom parts in your Wokwi project:

1. Add the following to your `wokwi.toml` file, replacing `[part-name]` and `v1.0.0` with the appropriate values:

```toml
[part-name]
url = "https://github.com/your-username/custom-wokwi-parts/releases/download/v1.0.0/part-name.chip.wokwi"
```

2. In your `diagram.json`, add the custom part:

```json
{
  "type": "part-name",
  "id": "my_part1",
  "top": 0,
  "left": 0,
  "attrs": {}
}
```

Replace `part-name` with the actual name of the part you're using.

## Usage

Each part has its own specific usage instructions. Please refer to the individual documentation for each part in the `docs` directory.

## Part-Specific Information

### ATECC608 Crypto Authentication Chip

- Pins: 3V3, GND, SCL, SDA
- Simulated functionalities include random number generation, key management, and cryptographic operations
- For detailed usage, see `docs/ATECC608.md`

(Add similar sections for other parts as they are included)

## Development

To modify or contribute to these custom parts:

1. Clone this repository
2. Navigate to the specific part's directory
3. Modify the source files as needed
4. Test your changes in a Wokwi project
5. Create a pull request with your improvements

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Wokwi for providing the custom part API and simulation environment
- All contributors to this project

## Disclaimer

These are simulated versions of various components for educational and prototyping purposes. They may not perfectly replicate the behavior of real hardware and should not be used in production environments or for critical applications.