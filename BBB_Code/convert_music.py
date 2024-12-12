def convert_audio_to_text(input_filename, output_filename):
    # Open the raw audio file in binary mode
    with open(input_filename, 'rb') as infile:
        # Read all data
        data = infile.read()
        
    # Open output text file
    with open(output_filename, 'w') as outfile:
        # Write each byte as a decimal number, one per line
        for byte in data:
            outfile.write(f"{byte}\n")

def main():
    # Get input and output filenames from user
    input_file = input("Enter the input RAW audio filename: ")
    output_file = input("Enter the output text filename: ")
    
    try:
        convert_audio_to_text(input_file, output_file)
        print(f"Successfully converted {input_file} to {output_file}")
    except FileNotFoundError:
        print(f"Error: Could not find input file {input_file}")
    except Exception as e:
        print(f"Error during conversion: {str(e)}")

if __name__ == "__main__":
    main()