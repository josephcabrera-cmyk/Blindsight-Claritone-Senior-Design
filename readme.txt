How to load program onto NUCLEO N657QX0 board with XIP (This assumes you have required software/firmware installed):

1. Open the project in STM32CubeIDE

2. Build the AppS and FSBL folders

3. Make sure STM32 Signing Tool is in environmental variables (Add to PATH: "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin")

4. Right click on debug folder of AppS and press "Open in terminal"

5. Copy and paste this command into terminal and press enter (make sure it signs successfully):

STM32_SigningTool_CLI.exe -bin Template_XIP_AppS.bin -nk -of 0x80000000 -t fsbl -o Template_XIP_AppS_Signed.bin -hv 2.3 -dump Template_XIP_AppS_Signed.bin -align

6. Right click on debug folder of FSBL and press "Open in terminal" 

7. Copy and paste this command into terminal and press enter (make sure it signs successfully):

STM32_SigningTool_CLI.exe -bin Template_XIP_FSBL.bin -nk -of 0x80000000 -t fsbl -o Template_XIP_FSBL_Signed.bin -hv 2.3 -dump Template_XIP_FSBL_Signed.bin -align

8. Go to STM32CubeProgrammer

9. Select the external loader from bottom left bottom called "MX66UW1G45G..." with 128M memory size

10. Go to Erasing and Programming on the left

11. Make sure board is in development mode (BOOT1 = 1) (JP2 is on pins 2-3). Disconnect, reconfigure, and connect if so.

12. Select the signed file Template_XIP_AppS_Signed.bin and program it at 0x70100000

13. Select the signed file Template_XIP_FSBL_Signed.bin and program it at 0x70000000

13. Disconnect board and place JP1 (BOOT0) and JP2 (BOOT1) on pins 1-2 for boot mode. Then reconnect to see running program.

