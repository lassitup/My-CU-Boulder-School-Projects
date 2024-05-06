/*
main.cpp
CSPB 1300 Image Processing Application

PLEASE FILL OUT THIS SECTION PRIOR TO SUBMISSION

- Your name:
    Justin Lassiter

- All project requirements fully met? (YES or NO):
    Yes

- If no, please explain what you could not get to work:
    N/A

- Did you do any optional enhancements? If so, please explain:
    - validation checks enabled for all user input
    - added a few additional image processes (Sepia Tone, Sobel Edge Detection, High Pass and Image Blend)
    
*/


#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits> // Added to be able to use code to clear input stream
#include <string> // Added for use of substr method
#include <algorithm> //Added for use of max_element function (Only used in alternative testing)

using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @param bytes  the number of bytes to read
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset, int bytes)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < bytes; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2, 4);
    int start = get_int(stream, 10, 4);
    int width = get_int(stream, 18, 4);
    int height = get_int(stream, 22, 4);
    int bits_per_pixel = get_int(stream, 28, 2);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * (bits_per_pixel / 8);
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel> (width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // We are ignoring the alpha channel if there is one

            // Advance the position to the next pixel
            pos = pos + (bits_per_pixel / 8);
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>>& image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//



/**
 * Rotate an image by 90 degrees
 * @param image    The input image to rotate
 * @return vector containing rotated image
 */
vector<vector<Pixel>> rotate_by_90(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    // use first loop to establish new vector?
    
    // loop through to size of original image width (the new height or # of rows)
    // This establishes the new vector image for the rotated image
    for (int row = 0; row < width; row++)
    {
        vector<Pixel> new_row(height); 
        new_image.push_back(new_row);   
    }
    
    
    for (int row = 0; row < height; row++) 
    {   
        for (int column = 0; column < width; column++)
        {
            Pixel old_pixel = image[row][column];
            //since vector established above, we can assigned to the appropriate position
            new_image[column][(height-1)-row] = old_pixel;
        }
    }
    
    return new_image;
}    



/**
 * Create a new image with a Vignette Effect applied (Process 1)
 * @param  image The input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    vector<vector<Pixel>> new_image; //declare vector to hold new image
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        new_image.push_back(new_row); //declare new row vector and nest within top level vector (creating new row)
        
        for (int column = 0; column < width; column++)
        {
           double distance = sqrt(pow((column - width/2.0), 2) + pow((row - height/2.0), 2));
           double scaling_factor = (height - distance)/height;
           //declare the new, empty Pixel
            Pixel new_pixel; 
            // add new pixel  
            new_image[row].push_back(new_pixel);    
            new_image[row][column].red = image[row][column].red * scaling_factor;
            new_image[row][column].green = image[row][column].green * scaling_factor;
            new_image[row][column].blue = image[row][column].blue * scaling_factor;
        }
    }
    return new_image;
}



/**
 * Create a new image with a Clarendon Effect applied (Process 2)
 * @param image  The input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel; 
            //could also set pixel first below and then append
            new_image[row].push_back(new_pixel); 
            double average_value = (image[row][column].red + image[row][column].green + image[row][column].blue) / 3.0;
            if (average_value >= 170)
            {
                new_image[row][column].red = (255 - (255 - image[row][column].red) * scaling_factor);
                new_image[row][column].green = (255 - (255 - image[row][column].green) * scaling_factor);
                new_image[row][column].blue = (255 - (255 - image[row][column].blue) * scaling_factor);
            }
            else if (average_value < 90)
            {
                new_image[row][column].red = image[row][column].red * scaling_factor;
                new_image[row][column].green = image[row][column].green * scaling_factor;
                new_image[row][column].blue = image[row][column].blue * scaling_factor;
            }
            else
            {
                new_image[row][column].red = image[row][column].red;
                new_image[row][column].green = image[row][column].green;
                new_image[row][column].blue = image[row][column].blue;
            }     
        } 
    }  
    return new_image;
}



/**
 * Create a new image with a Grayscale Effect applied (Process 3)
 * @param  image The input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_3(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            Pixel new_pixel; //declare the new, empty Pixel
            int gray_value = (image[row][column].red + image[row][column].green + image[row][column].blue) / 3.0;
            new_pixel.red = gray_value;
            new_pixel.green = gray_value;
            new_pixel.blue = gray_value;
            new_image[row].push_back(new_pixel);
        }
    }
    
    return new_image;
}



/**
 * Create a new image rotated by 90 degrees clockwise (Process 4)
 * @param  image The input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_4(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    
    // loop through to size of original image width (the new height or # of rows)
    // This establishes the new vector image "frame" or structure for the rotated image
    for (int row = 0; row < width; row++)
    {
        vector<Pixel> new_row(height); 
        //since vector established above, we can assigned the Pixel to the appropriate position in the new Vector below
        new_image.push_back(new_row);   
    }
    
    for (int row = 0; row < height; row++) 
    {   
        for (int column = 0; column < width; column++)
        {
            Pixel old_pixel = image[row][column];
            new_image[column][(height-1)-row] = old_pixel;
        }
    }
    
    return new_image;
}



/**
 * Creates a copy of the provided image rotated in increments of 90 degrees 
   using the quantity provided by user (Process 5)
 * @param  image The input image to process
 * @param number the number of times to rotate the image clockwise
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_5(const vector<vector<Pixel>>& image, int number)
{
    // number paramter variable is an integer, therefore value is inevitable a multiple of 90
    int angle = number * 90;
    
    if (angle % 360 == 0)
    {
        return image;
    }
    else if (angle % 360 == 90)
    {
        return rotate_by_90(image);
    }
    else if (angle % 360 == 180)
    {
        return rotate_by_90(rotate_by_90(image));
    }
    else
    {
        return rotate_by_90(rotate_by_90(rotate_by_90(image)));
    }
    return image; 
}



/**
 * Produces a copy of image with expanded height and/or width based on user provided x and y values (Process 6)
 * @param  image The input image to process
 * @param x_scale amount to multiply original image width and expand x axis value of new image
 * @param y_scale amount to multiply original image height and expand y axis value of new image
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_6(const vector<vector<Pixel>>& image, int x_scale, int y_scale)
{
    //return the number of rows (current y value)
    int height = image.size(); 
    //return the number of columns (current x value)
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    
    for (int row = 0; row < (height * y_scale); row++)
    {
        vector<Pixel> new_row (width * x_scale); 
        //Declare new row vector and nest within top level vector (creating new row of columns)
        new_image.push_back(new_row); 
    }
    
    for (int row = 0; row < new_image.size(); row++) 
    {
         for (int column = 0; column < new_image[0].size(); column++)
         { 
             Pixel old_pixel = image[row/y_scale][column/x_scale];
             new_image[row][column] = old_pixel;
         }   
    }
    
    return new_image;
}



/**
 * Create a new image with high contrast effect applied (Process 7)
 * @param  image The input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_7(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    int height = image.size(); 
    //return the number of columns
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel;
            
            //extract the color values of the original image
            int red = image[row][column].red;
            int green = image[row][column].green;
            int blue = image[row][column].blue;
            
            int new_red, new_green, new_blue;
            
            //using type double to get more accurate comparison below
            double gray_value = (red + green + blue) / 3.0; 
            
            if (gray_value >= 255/2.0)
            {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            else
            {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }                
            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel); 
        }
    } 
    
    return new_image;
}



/**
 * Create a new image with lighten effect applied (Process 8)
 * @param  image the input image to process
 * @param scaling_factor amount used to lighten the pixel value 
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_8(const vector<vector<Pixel>>& image, double scaling_factor)
{
    //return the number of rows
    int height = image.size(); 
    //return the number of columns
    
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {

            Pixel new_pixel; //declare the new, empty Pixel
            
            //extract the color values of the original image
            int red = image[row][column].red;
            int green = image[row][column].green;
            int blue = image[row][column].blue;
            
            int new_red, new_green, new_blue;
           
            //apply scaling factor
            new_red = 255 - (255 - red) * scaling_factor;
            new_green = 255 - (255 - green) * scaling_factor;
            new_blue = 255 - (255 - blue) * scaling_factor;

            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel); 
        }
    }     
    return new_image;   
}



/**
 * Create a new image with darken effect applied (Process 9)
 * @param  image the input image to process
 * @param scaling_factor amount used to darken the pixel value 
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_9(const vector<vector<Pixel>>& image, double scaling_factor)
{
    //return the number of rows
    int height = image.size(); 
    //return the number of columns
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            //extract the color values of the original image
            int red = image[row][column].red;
            int green = image[row][column].green;
            int blue = image[row][column].blue;
            
            int new_red, new_green, new_blue;
           
            // apply scaling factor
            new_red = red * scaling_factor;
            new_green = green * scaling_factor;
            new_blue = blue * scaling_factor;

            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel); 
        }
    }     
    return new_image;     
}



/**
 * Create a new image with only black, white, red, green and blue colors (Process 10)
 * @param  image the input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_10(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    int height = image.size(); 
    //return the number of columns
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            //extract the color values of the original image
            int red = image[row][column].red;
            int green = image[row][column].green;
            int blue = image[row][column].blue;
            
            int max_value;
            
            
            /* Tree to determine max value, red is the catch all in the event two or more values are equal
            If blue and green are the same or red truly is the largest, the else branch will trigger - the results of this method 
            agrees with the sample image result. The max value obtained from using the max_element member function returns
            a slightly different result due to the actual max being returned. It is possible that the red value is used as 
            max value in the condition tree below even if it is less than the blue and green values as it is used as a catch all. 
            For example, if blue and green are the same, red will be used regardless if it is the max*/
            
            //Alternative method - not used
            /*vector<int> color_values = {red, green, blue};
            int max_value = *max_element(color_values.begin(), color_values.end());*/
            
            
            if (blue > green && blue > red)
            {
                max_value = blue;
            }
            else if (green > red && green > blue)
            {
                max_value = green;
            }
            else
            {
                max_value = red;
            }
            
            //declare new, replacement color variables
            int new_red, new_green, new_blue;
            
            if (red + green + blue >= 550)
            {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            else if (red + green + blue <= 150)
            {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }
            else if (max_value == red)
            {
                new_red = 255;
                new_green = 0;
                new_blue = 0;
            }
            else if (max_value == green)
            {
                new_red = 0;
                new_green = 255;
                new_blue = 0;
            }
            else
            {
                new_red = 0;
                new_green = 0;
                new_blue = 255;
            }
            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel); 
        }
    }     
    return new_image;  
}



/**
 * Create a new image with sepia tone effect applied (Process 11)
 * @param  image the input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_11(const vector<vector<Pixel>>& image)
{
    //return the number of rows
    int height = image.size(); 
    //return the number of columns
    int width = image[0].size(); 
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            //extract the color values of the original image
            int red = image[row][column].red;
            int green = image[row][column].green;
            int blue = image[row][column].blue;
            
            int new_red, new_green, new_blue;
           
            
            new_red = (red * 0.393 + green * 0.769 + blue * 0.189);
            new_green = (red * 0.349 + green * 0.686 + blue * 0.168);
            new_blue = (red * 0.272 + green * 0.534 + blue * 0.131);
            
            //check to ensure that a value isn't set greater than the max value of 255
            if (new_red > 255) {new_red = 255;}
            if (new_green > 255) {new_green = 255;}
            if (new_blue > 255) {new_blue = 255;}
            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel); 
        }
    }     
    return new_image;     
}



/**
 * Create a new image with sobel edge detection applied (Process 12)
 * @param  image the input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_12(const vector<vector<Pixel>>& image)
{
    int height = image.size(); //return the number of rows
    int width = image[0].size(); //return the number of columns
    vector<vector<Pixel>> new_image; //declare vector to hold new image
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {

            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            //extract the color values of the original image

            
            double new_red, new_green, new_blue;
            
            //This branch handles the edge / border pixels - all border pixels are assigned the original image pixel
            if (row == 0 || row == height - 1 || column == 0 || column == width - 1)
            {

                new_red = image[row][column].red;
                new_green = image[row][column].green;
                new_blue = image[row][column].blue;
            }
            else
            {                
                //declare and initialize variables to aggregate the sum of the sobel edge detection kernel values (Gx and Gy) being applied to all nine pixels
                int gsum_redx = 0;
                int gsum_greenx = 0;
                int gsum_bluex = 0;

                int gsum_redy = 0;
                int gsum_greeny = 0;
                int gsum_bluey = 0;

                //nested for loops used to determine each pixel within the Kernel bracket and apply the appropriate kernel value to each applicable pixel
                //each pixel value is added to the aggregation variables above
                for (int r = row-1; r <= row + 1; r++)
                {
                    for (int c = column - 1; c <= column + 1; c++)
                    {
                        if (r == row-1 && c == column-1)
                        {
                            gsum_redx = gsum_redx + image[r][c].red * 1;
                            gsum_greenx = gsum_greenx + image[r][c].green * 1;
                            gsum_bluex = gsum_bluex + image[r][c].blue * 1;
                    
                            gsum_redy = gsum_redy + image[r][c].red * 1;
                            gsum_greeny = gsum_greeny + image[r][c].green * 1;
                            gsum_bluey = gsum_bluey + image[r][c].blue * 1;
                        }      
                        else if (r == row-1 && c == column)
                        {
                            gsum_redx += image[r][c].red * 0;
                            gsum_greenx += image[r][c].green * 0;
                            gsum_bluex += image[r][c].blue * 0;


                            gsum_redy += image[r][c].red * 2;
                            gsum_greeny += image[r][c].green * 2;
                            gsum_bluey += image[r][c].blue * 2;                      
                        }
                        else if (r == row-1 && c == column+1)
                        {
                            gsum_redx += image[r][c].red * -1;
                            gsum_greenx += image[r][c].green * -1;
                            gsum_bluex += image[r][c].blue * -1;


                            gsum_redy += image[r][c].red * 1;
                            gsum_greeny += image[r][c].green * 1;
                            gsum_bluey += image[r][c].blue * 1;                        
                        }
                        else if (r == row && c == column-1)
                        {
                            gsum_redx += image[r][c].red * 2;
                            gsum_greenx += image[r][c].green * 2;
                            gsum_bluex += image[r][c].blue * 2;

    
                            gsum_redy += image[r][c].red * 0;
                            gsum_greeny += image[r][c].green * 0;
                            gsum_bluey += image[r][c].blue * 0;  
                        }                        
                         else if (r == row && c == column)
                        {
                            gsum_redx += image[r][c].red * 0;
                            gsum_greenx += image[r][c].green * 0;
                            gsum_bluex += image[r][c].blue * 0;


                            gsum_redy += image[r][c].red * 0;
                            gsum_greeny += image[r][c].green * 0;
                            gsum_bluey += image[r][c].blue * 0; 
                        }                                             
                         else if (r == row && c == column+1)
                        {
                            gsum_redx += image[r][c].red * -2;
                            gsum_greenx += image[r][c].green * -2;
                            gsum_bluex += image[r][c].blue * -2;


                            gsum_redy += image[r][c].red * 0;
                            gsum_greeny += image[r][c].green * 0;
                            gsum_bluey += image[r][c].blue * 0;
                        }                       
                        else if (r == row+1 && c == column-1)
                        {
                            gsum_redx += image[r][c].red * 1;
                            gsum_greenx += image[r][c].green * 1;
                            gsum_bluex += image[r][c].blue * 1;


                            gsum_redy += image[r][c].red * -1;
                            gsum_greeny += image[r][c].green * -1;
                            gsum_bluey += image[r][c].blue * -1;
                        }   
                        else if (r == row+1 && c == column)
                        {   
                            gsum_redx += image[r][c].red * 0;
                            gsum_greenx += image[r][c].green * 0;
                            gsum_bluex += image[r][c].blue * 0;


                            gsum_redy += image[r][c].red * -2;
                            gsum_greeny += image[r][c].green * -2;
                            gsum_bluey += image[r][c].blue * -2;
                        }                         
                        else if (r == row+1 && c == column+1)
                        {   
                            gsum_redx += image[r][c].red * -1;
                            gsum_greenx += image[r][c].green * -1;
                            gsum_bluex += image[r][c].blue * -1;


                            gsum_redy += image[r][c].red * -1;
                            gsum_greeny += image[r][c].green * -1;
                            gsum_bluey += image[r][c].blue * -1;
                        }                         
                    }
                       
                }
                
                //perform formulas to determine G values for new pixel (this is the value that will be assigned to the new pixel)
                new_red = sqrt(pow(gsum_redx, 2) + pow(gsum_redy, 2));
                new_green = sqrt(pow(gsum_greenx, 2) + pow(gsum_greeny, 2));
                new_blue = sqrt(pow(gsum_bluex, 2) + pow(gsum_bluey, 2));
                
            }
            
            //check to ensure that a value isn't set greater than the max value of 255
            if (new_red > 255) {new_red = 255;}
            if (new_green > 255) {new_green = 255;}
            if (new_blue > 255) {new_blue = 255;}
            if (new_red < 0) {new_red = 0;}
            if (new_green < 0) {new_green = 0;}
            if (new_blue < 0) {new_blue = 0;}
            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel);    
        }
    }     
    return new_image;     
}



/**
 * Create a new image with Hi-Pass (sharpening) applied (Process 13)
 * @param  image the input image to process
 * @return  vector containing the new, altered image
 */
vector<vector<Pixel>> process_13(const vector<vector<Pixel>>& image)
{
    int height = image.size(); //return the number of rows
    int width = image[0].size(); //return the number of columns
    vector<vector<Pixel>> new_image; //declare vector to hold new image
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {

            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            //extract the color values of the original image

            
            double new_red, new_green, new_blue;
            
            //This branch handles the edge / border pixels - all border pixels are assigned the original image pixel
            if (row == 0 || row == height - 1 || column == 0 || column == width - 1)
            {

                new_red = image[row][column].red;
                new_green = image[row][column].green;
                new_blue = image[row][column].blue;
            }
            else
            {                
                //declare and initialize variables to aggregate the sum of the sobel edge detection kernel values (Gx and Gy) being applied to all nine pixels
                int gsum_red = 0;
                int gsum_green = 0;
                int gsum_blue = 0;

                //nested for loops used to determine each pixel within the Kernel bracket and apply the appropriate kernel value to each applicable pixel
                //each pixel value is added to the aggregation variables above
                for (int r = row-1; r <= row + 1; r++)
                {
                    for (int c = column - 1; c <= column + 1; c++)
                    {
                        if (r == row-1 && c == column-1)
                        {
                            gsum_red = gsum_red + image[r][c].red * 0;
                            gsum_green = gsum_green + image[r][c].green * 0;
                            gsum_blue = gsum_blue + image[r][c].blue * 0;
                        }      
                        else if (r == row-1 && c == column)
                        {
                            gsum_red += image[r][c].red * (-1/4.0);
                            gsum_green += image[r][c].green * (-1/4.0);
                            gsum_blue += image[r][c].blue * (-1/4.0);
                   
                        }
                        else if (r == row-1 && c == column+1)
                        {
                            gsum_red += image[r][c].red * 0;
                            gsum_green += image[r][c].green * 0;
                            gsum_blue += image[r][c].blue * 0;                     
                        }
                        else if (r == row && c == column-1)
                        {
                            gsum_red += image[r][c].red * (-1/4.0);
                            gsum_green += image[r][c].green * (-1/4.0);
                            gsum_blue += image[r][c].blue * (-1/4.0);  
                        }                        
                         else if (r == row && c == column)
                        {
                            gsum_red += image[r][c].red * 2;
                            gsum_green += image[r][c].green * 2;
                            gsum_blue += image[r][c].blue * 2;
                        }                                             
                         else if (r == row && c == column+1)
                        {
                            gsum_red += image[r][c].red * (-1/4.0);
                            gsum_green += image[r][c].green * (-1/4.0);
                            gsum_blue += image[r][c].blue * (-1/4.0);
                        }                       
                        else if (r == row+1 && c == column-1)
                        {
                            gsum_red += image[r][c].red * 0;
                            gsum_green += image[r][c].green * 0;
                            gsum_blue += image[r][c].blue * 0;
                        }   
                        else if (r == row+1 && c == column)
                        {   
                            gsum_red += image[r][c].red * (-1/4.0);
                            gsum_green += image[r][c].green * (-1/4.0);
                            gsum_blue += image[r][c].blue * (-1/4.0);
                        }                         
                        else if (r == row+1 && c == column+1)
                        {   
                            gsum_red += image[r][c].red * 0;
                            gsum_green += image[r][c].green * 0;
                            gsum_blue += image[r][c].blue * 0;
                        }                         
                    }
                       
                }
                
                //perform formulas to determine G values for new pixel (this is the value that will be assigned to the new pixel)
                /*new_red = sqrt(pow(gsum_redx, 2) + pow(gsum_redy, 2));
                new_green = sqrt(pow(gsum_greenx, 2) + pow(gsum_greeny, 2));
                new_blue = sqrt(pow(gsum_bluex, 2) + pow(gsum_bluey, 2));*/
                
                new_red = gsum_red;
                new_green = gsum_green;
                new_blue = gsum_blue;
                
            }
            
            //check to ensure that a value isn't set greater than the max value of 255
            if (new_red > 255) {new_red = 255;}
            if (new_green > 255) {new_green = 255;}
            if (new_blue > 255) {new_blue = 255;}
            if (new_red < 0) {new_red = 0;}
            if (new_green < 0) {new_green = 0;}
            if (new_blue < 0) {new_blue = 0;}
            
            //set the colors amounts of the newly created pixel
            new_pixel.red = new_red;
            new_pixel.green = new_green;
            new_pixel.blue = new_blue;
        
            // add new pixel to the row 
            new_image[row].push_back(new_pixel);    
        }
    }     
    return new_image;     
}



/**
 * Create a new image by blending two input images (Process 14)
 * @param  image The input image to process
 * @param second image to use in blending 
 * @return  vector containing the new, altered image
 */
//note to self, come back after submission with more time and add test to ensure equal size and use enlarge function if needed.
vector<vector<Pixel>> process_14(const vector<vector<Pixel>>& image, const vector<vector<Pixel>>& image2)
{
    //return the number of rows
    double height = image.size(); 
    //return the number of columns
    double width = image[0].size(); 
    
    
    double height2 = image2.size();
    double width2 = image2[0].size();
    
    //declare vector to hold new image
    vector<vector<Pixel>> new_image; 
    
    for (int row = 0; row < height; row++)
    {
        vector<Pixel> new_row; 
        //declare new row vector and nest within top level vector (creating new row)
        new_image.push_back(new_row); 
        
        for (int column = 0; column < width; column++)
        {
            //declare the new, empty Pixel
            Pixel new_pixel; 
            
            if (row >= height2)
            {
                new_pixel.red = image[row][column].red;
                new_pixel.green = image[row][column].green;
                new_pixel.blue = image[row][column].blue;              
            }
            else if (column >= width2)
            {
                new_pixel.red = image[row][column].red;
                new_pixel.green = image[row][column].green;
                new_pixel.blue = image[row][column].blue;
            }
            else
            {
                int red1 = image[row][column].red;
                int green1 = image[row][column].green;
                int blue1 = image[row][column].blue;
                
                int red2 = image2[row][column].red;
                int green2 = image2[row][column].green;
                int blue2 = image2[row][column].blue;
                                
                new_pixel.red = (red1 + red2) / 2;
                new_pixel.green = (green1 + green2) / 2;
                new_pixel.blue = (blue1 + blue2) / 2;              
            }  
             new_image[row].push_back(new_pixel);   
        }
    }
    return new_image;
}



/**
 * Prompts user to input file name of the source image and performs validation check
 * @return string  name of source file
 */
string source_file_name()
{
   string file_name;
   do
   {
      cout << "Enter input BMP filename: ";
      cin >> file_name;
       // exit the loop if a file name with ".bmp" is provided by the user
      if (file_name.length() >= 5 && file_name.substr(file_name.length()- 4) == ".bmp") {break;} 
       //prompts warning to user that the value they entered was not the correct format
      cout << endl << "Please enter a valid name! (Desired name followed by \".bmp\") - e.g. \"example.bmp\"" << endl << endl;  
       
   }
   while (true);
    
   return file_name;
}



/**
 * Prompts user to input file name of the resulting new image and performs validation check
 * @return name of output file
 */
string new_file_name()
{
   string file_name;
    
   do
   {
        cout << "Enter output BMP filename: ";
        cin >> file_name;
        cout << endl;
        // exit the loop if a file name with ".bmp" is provided by the user
        if (file_name.length() >= 5 && file_name.substr(file_name.length()- 4) == ".bmp") {break;} 
        //prompts warning to user that the value they entered was not the correct 
        cout << "Please enter a valid name! (Desired name followed by \".bmp\") - e.g. \"example.bmp\"" << endl << endl;
       
   }
   while (true);
    
   return file_name;
}



/**
 * Prompts user to input a scaling_factor of type double and performs validation checks
 * @return the user provided scaling factor 
 */
double scaling_factor_validate()
{
    double scaling_factor;
    do
    {
        cout << "Enter scaling factor: ";
        cin >> scaling_factor;  
        
        cout << endl;
        
        string catch_extra; 
        //used to catch any extra values inputted by the user still remaining in the stream
        getline(cin, catch_extra);
    
         // checking to determine if the input is numeric and reset cin stream if not
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); //clears out the input stream 
            cout << "You've entered an invalid factor. Please enter a positive numeric value (i.e 1 or 1.0).";
            cout << endl << endl;

        }      
        else if (catch_extra.length() > 0)
        {
            cout << "You've entered an invalid factor. Please enter a positive numeric value (i.e 1 or 1.0).";  
            cout << endl << endl;
        }
        else if (scaling_factor < 0)
        {
            cout << "You've entered an invalid factor. Please enter a positive numeric value (i.e 1 or 1.0).";
            cout << endl << endl;
            //Break current loop cycle and force restart to beginning of loop
            continue; 
        }
        else
        {
            return scaling_factor;
        }
    }
    while(true);
}



/**
 * Prompts user to input an integer value and performs validation checks
 * @return the user provided integer 
 */
int integer_input_validate()
{
    //read in as a string first to allow check for decimal
    string input; 
    do
    {
        cin >> input;   
        // check to see if value provided was a double - fails and restarts
        if (input.find(".") != string::npos)
        {
            //cin.ignore(numeric_limits<streamsize>::max(), '\n'); //clears out the input stream 
            cout << endl << "You've entered an invalid value. Please enter a positive integer value: ";
        }
        else
        {
            try
            {
                size_t not_converted;
                // use of second parameter in stoi member function to determine if full string input was successfully converted to an int
                // if first value not converted does not equal string length, invalid characters are included and deemed invalid
                // user is required to enter a true integer value, otherwise I am considering it an error on the user
                int converted_int = stoi(input, &not_converted);
                if ((not_converted) != input.length())
                {
                    cout << endl << "You've entered an invalid value. Please enter a positive integer value: ";
                    //Break current loop cycle and force restart to beginning of loop
                    continue; 
                }
                //ensure int value provided is 0 or greater
                else if (converted_int < 0)
                {
                    cout << endl << "You've entered an invalid value. Please enter a postive integer value: ";
                    //Break current loop cycle and force restart to beginning of loop
                    continue;  
                }
                cout << endl;  
                return converted_int;
            } 
            catch (...)
            {
                cout << endl <<"You've entered an invalid value. Please enter a positive integer value: ";
            }    
        }
    }
    while(true);
}            



/**
 * Prompts user to input an integer value between 0 and 12 and performs validation checks
 * @return the user provided integer or returns to main function if non-decimal character provided
 */
int interface_input_validate()
{  
        do
        {
            // double type allows user to input int as a double value (i.e. 2.0) and still be able to proceed
            double menu_select_input; 
            cin >> menu_select_input; 

            string input_to_validate; 

            string catch_extra; 
            //used to catch any extra values inputted by the user still remaining in the stream
            getline(cin, catch_extra);

            // If user enters a character first, regardless of any excess input in the stream, the program will terminate.
            if (cin.fail()) 
            {
                //if cin enters a failed state, exit function and return control back to main where the while condition will then fail
                return -1; 
            }
            else if (catch_extra.length() > 0)
            {
                cout << endl << "You've entered an invalid value. Please enter an integer value between 0 and 14 or \"Q\" to quit: ";
                continue;
            }
            else 
            { 

                input_to_validate = to_string(menu_select_input);
                int decimal_location = input_to_validate.find(".");

                string test = "";
                string truncated_string = "";

                for (int i = decimal_location + 1; i < input_to_validate.length(); i++)
                {
                    test = test + input_to_validate[i];
                }
                for (int i = 0; i < decimal_location; i++)
                {
                    truncated_string = truncated_string + input_to_validate[i];
                }

                //to_string returns only 6 digits post decimal; therefore, all digits should be 0
                if (test != "000000")
                {
                    cout << endl << "You've entered an invalid value. Please enter an integer value between 0 and 14 or \"Q\" to quit: ";
                    continue;
                }   

                try
                    {
                    int converted_int = stoi(truncated_string);
                    if (converted_int < 0 || converted_int > 14)
                    {
                        cout << endl << "You've entered an invalid value. Please enter an integer value between 0 and 14 or \"Q\" to quit: ";
                        continue;
                    }
                    return converted_int;
                    } 
                    catch (...)
                    {
                        cout << endl << "You've entered an invalid value. Please enter an integer value between 0 and 14 or \"Q\" to quit: ";
                        continue;          
                    }       
            }
        }
        while (true);  
}



int main()
{
    cout << endl << "CSPB 1300 Image Processing Application" << endl << endl;
    string input_file = source_file_name();
    vector<vector<Pixel>> source_image = read_image(input_file); 
    
    // do loop used to get the initial user input value then check in while loop for failed status (Q)                                               
    do
    {   
        cout << endl << "Image Processing Menu" << endl << endl;
        cout << 
        "0)  Change Image" << " (Current File: " << input_file << ")\n"
        "1)  Vignette\n"
        "2)  Clarendon\n"
        "3)  Grayscale\n"
        "4)  Rotate 90 Degrees\n"
        "5)  Rotate Multiple 90 Degrees\n"
        "6)  Enlarge\n"
        "7)  High Contrast\n"
        "8)  Lighten\n"
        "9)  Darken\n"
        "10) Black, White, Red, Green, Blue\n"
        "11) Sepia Tone\n"
        "12) Sobel Edge Detection\n"
        "13) High Pass (Sharpen)\n" 
        "14) Image Blend\n";

        cout << endl << "Enter menu selection (Q to quit): ";
        int menu_select = interface_input_validate();
        
        cout << endl;
        
        // declare variable to hold new image
        vector<vector<Pixel>> new_image; 
        //declare variable to hold the returned status after process is complete
        bool result_status; 
        //declare variable to store the name of the output file (new file name)
        string output_file;
        // declare, initialize scaling factor here - used in multiple branches below
        double scaling_factor = 0; 
        
        //include test for cin.fail() status to ensure 0 value is not triggered from false result above (via menu_select "Q" selected)
        if (menu_select == 0) // && !cin.fail() - removed this as -1 is returned in the new function
        {
            cout << "Change Image selected" << endl << endl;
            input_file = source_file_name();
            source_image = read_image(input_file);
        }
        else if (menu_select == 1)
        {
            cout << "Vignette selected" << endl << endl;
            new_image = process_1(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully Applied Vignette!" << endl;}
            else {cout << "Operation failed" << endl;}
        }
        else if (menu_select == 2)
        {
            cout << "Clarendon selected" << endl << endl;
            scaling_factor = scaling_factor_validate();
            new_image = process_2(source_image, scaling_factor);
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully Applied Clarendon!" << endl;}
            else {cout << "Operation failed" << endl;}
            
        }
        else if (menu_select == 3)
        {
            cout << "Grayscale selected" << endl << endl;
            new_image = process_3(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully Applied Grayscale!" << endl;}
            else {cout << "Operation failed" << endl;}
        }        
        else if (menu_select == 4)
        {
            cout << "Rotate 90 Degrees selected" << endl << endl;
            new_image = process_4(source_image);
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully Rotated 90 Degrees!" << endl;}
            else {cout << "Operation failed" << endl;}
        }         
        else if (menu_select == 5)
        {
            cout << "Rotate Multiple 90 Degrees selected" << endl << endl;
            cout << "Enter the number of rotations: ";
            int rotations = integer_input_validate();
            new_image = process_5(source_image, rotations);
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied the selected rotations!" << endl;}
            else {cout << "Operation failed" << endl;}
        } 
        else if (menu_select == 6)
        {
            cout << "Enlarge selected" << endl << endl;
            cout << "Enter x scale: ";
            int x_select = integer_input_validate();
            cout << "Enter y scale: ";
            int y_select = integer_input_validate();
            new_image = process_6(source_image, x_select, y_select);
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully enlarged!" << endl;}
            else {cout << "Operation failed" << endl;}
        }        
        else if (menu_select == 7)
        {
            cout << "High Contrast selected" << endl << endl;
            new_image = process_7(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied High Contrast!" << endl;}
            else {cout << "Operation failed" << endl;}
        }             
        else if (menu_select == 8)
        {
            cout << "Lighten selected" << endl << endl;
            scaling_factor = scaling_factor_validate();
            new_image = process_8(source_image, scaling_factor); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Lighten!" << endl;}
            else {cout << "Operation failed" << endl;}
        }        
        else if (menu_select == 9)
        {
            cout << "Darken selected" << endl << endl;
            scaling_factor = scaling_factor_validate();
            new_image = process_9(source_image, scaling_factor); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Darken!" << endl;}
            else {cout << "Operation failed" << endl;}
        }  
        else if (menu_select == 10)
        {
            cout << "Black, White, Red, Green, Blue selected" << endl << endl;
            new_image = process_10(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Black, White, Red, Green, Blue!" << endl;}
            else {cout << "Operation failed" << endl;}
        }  
        else if (menu_select == 11)
        {
            cout << "Sepia Tone selected" << endl << endl;
            new_image = process_11(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Sepia Tone!" << endl;}
            else {cout << "Operation failed" << endl;}
        }  
        else if (menu_select == 12)
        {
            cout << "Sobel Edge Detection selected" << endl << endl;
            new_image = process_12(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Sobel Edge Detection!" << endl;}
            else {cout << "Operation failed" << endl;}
        }   
         else if (menu_select == 13)
        {
            cout << "High Pass selected" << endl << endl;
            new_image = process_13(source_image); 
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied High Pass!" << endl;}
            else {cout << "Operation failed" << endl;}
        }    
         else if (menu_select == 14)
        {
            cout << "Image Blend selected" << endl << endl;
            
            // call on user to provide a second image to use
            cout << "Enter Second Image name to be used for blending" << endl << endl;
            string second_image_name = source_file_name();
            vector<vector<Pixel>> second_image = read_image(second_image_name); 
            new_image = process_14(source_image, second_image); 
            cout << endl;
            output_file = new_file_name();
            result_status = write_image(output_file, new_image);
            if (result_status == 1) {cout << "Successfully applied Image Blend!" << endl;}
            else {cout << "Operation failed" << endl;}
        }    
        
    }
    while (!cin.fail()); //loop to continue if cin not in failed state
    
    cout << "Thank you for using my program! Enjoy the pictures!" << endl << endl;
    cout << "Now Exiting Program..." << endl << endl;
    
    return 0;
}