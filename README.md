# DbgKeystone
A Keystone engine powered Windows Debugger extension

## Reasoning

WinDbg's default assember (the `a` command) can't handle instructions involving x64 registers and many instruction set extensions. For example: <br>

<p align="center">
  <img src="https://user-images.githubusercontent.com/16905064/135732636-a5af60b6-fb41-4a59-b19a-1f0cb1c21ec2.PNG">
</p>

With this extension you are able to assemble anything that the Keystone engine supports:
<p align="center">
  <img src="https://user-images.githubusercontent.com/16905064/136116591-c020ee15-a27f-4cfd-a1ab-500d361c5d05.gif">
</p>

## Usage
* The easiest way to use this is to simply download the binary file in the `Binary` directory of this repo.
* Load the extension in WinDbg with the `.load [path to extension]`

* You can now use `!a [address]` which will assemble using Keystone 

## Compiling
* Download the Keystone Engine's Win64 pacakge [here](https://www.keystone-engine.org/download/)
* Create a new Visual Studio solution and include the Keystone engine headers and the static library.
* Add the source code of this repository
* Compile, and load the plugin  `.load [path to extension]`
