/*
 * Copyright (C) 2012-2016 Jacob R. Lifshay
 * This file is part of Voxels.
 *
 * Voxels is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Voxels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Voxels; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include "resource.h"
#include <iostream>

int main()
{
    using namespace programmerjake::voxels;
    try
    {
        auto inputStream = resource::ResourceManager().readResource("folder1/file1.txt");
        while(true)
        {
            constexpr std::size_t bufferSize = 256;
            unsigned char buffer[bufferSize];
            auto readSize = inputStream->readAllBytes(buffer, bufferSize, false);
            std::cout.write(reinterpret_cast<const char *>(buffer), readSize);
            if(readSize == 0)
                break;
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
