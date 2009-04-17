=============================================================================
TWL Fingerprinter
=============================================================================

Table of Contents
- Introduction
- Package Structure
- Required Environment
- Using the tool
- Licences
- Revision History

=======================================================================
Introduction
=======================================================================

This package includes the TWL Fingerprinter, a tool used to 
register the fingerprinter in the TWL ROM.

=======================================================================
Package Structure
=======================================================================

FingerprintTWLSetup.msi : Installer

=======================================================================
Required Environment
=======================================================================

Microsoft Windows XP SP2
Microsoft .NET Framework 2.0

Store following program and DLLs, which are not included in this package,
into the install folder.
  - maketad.exe (included in TwlSDK)
  - cygwin1.dll (included in Cygwin)
  - cygcrypto-0.9.8.dll (included in OpenSSL)

=======================================================================
Using the Tool
=======================================================================

The TWL Fingerprinter tool has GUI mode and CUI(Console) mode.

GUI mode:

  Execute either an icon on the desktop or on the install folder.

CUI mode:

  Type following words on the command prompt and see usage.

  > FingerprinterTWL.exe -h

=======================================================================
Licenses
=======================================================================

This product includes software developed by the OpenSSL Project
 for use in the OpenSSL Toolkit (http://www.openssl.org/)

=======================================================================
Revision History
=======================================================================
Version 1.0 (2009/04/17)
- Initial version.
