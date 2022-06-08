
secret:     file format elf64-x86-64


Disassembly of section .text:

0000000000400940 <.text>:
  400940:	41 54                	push   %r12
  400942:	31 c0                	xor    %eax,%eax
  400944:	be 02 00 00 00       	mov    $0x2,%esi
  400949:	bf 80 0e 40 00       	mov    $0x400e80,%edi
  40094e:	55                   	push   %rbp
  40094f:	53                   	push   %rbx
  400950:	48 83 ec 10          	sub    $0x10,%rsp
  400954:	e8 d7 ff ff ff       	call   400930 <open@plt>
  400959:	83 f8 ff             	cmp    $0xffffffff,%eax
  40095c:	89 c3                	mov    %eax,%ebx
  40095e:	0f 84 ab 01 00 00    	je     400b0f <open@plt+0x1df>
  400964:	31 f6                	xor    %esi,%esi
  400966:	31 c0                	xor    %eax,%eax
  400968:	bf 8b 0e 40 00       	mov    $0x400e8b,%edi
  40096d:	e8 be ff ff ff       	call   400930 <open@plt>
  400972:	83 f8 ff             	cmp    $0xffffffff,%eax
  400975:	89 c5                	mov    %eax,%ebp
  400977:	0f 84 e3 00 00 00    	je     400a60 <open@plt+0x130>
  40097d:	be e0 0e 40 00       	mov    $0x400ee0,%esi
  400982:	89 c7                	mov    %eax,%edi
  400984:	e8 87 ff ff ff       	call   400910 <fdopen@plt>
  400989:	48 85 c0             	test   %rax,%rax
  40098c:	48 89 c3             	mov    %rax,%rbx
  40098f:	0f 84 00 02 00 00    	je     400b95 <open@plt+0x265>
  400995:	48 c7 44 24 08 00 00 	movq   $0x0,0x8(%rsp)
  40099c:	00 00 
  40099e:	e8 6d fe ff ff       	call   400810 <__errno_location@plt>
  4009a3:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  4009a8:	c7 00 00 00 00 00    	movl   $0x0,(%rax)
  4009ae:	be f8 0e 40 00       	mov    $0x400ef8,%esi
  4009b3:	31 c0                	xor    %eax,%eax
  4009b5:	48 89 df             	mov    %rbx,%rdi
  4009b8:	e8 63 fe ff ff       	call   400820 <__isoc99_fscanf@plt>
  4009bd:	83 e8 01             	sub    $0x1,%eax
  4009c0:	7e 0a                	jle    4009cc <open@plt+0x9c>
  4009c2:	bf 05 00 00 00       	mov    $0x5,%edi
  4009c7:	e8 d4 02 00 00       	call   400ca0 <open@plt+0x370>
  4009cc:	41 bc ef be ad de    	mov    $0xdeadbeef,%r12d
  4009d2:	4c 33 64 24 08       	xor    0x8(%rsp),%r12
  4009d7:	31 c0                	xor    %eax,%eax
  4009d9:	e8 e2 02 00 00       	call   400cc0 <open@plt+0x390>
  4009de:	4c 29 e0             	sub    %r12,%rax
  4009e1:	48 3d 10 0e 00 00    	cmp    $0xe10,%rax
  4009e7:	0f 87 44 01 00 00    	ja     400b31 <open@plt+0x201>
  4009ed:	e8 6e fe ff ff       	call   400860 <getuid@plt>
  4009f2:	89 c7                	mov    %eax,%edi
  4009f4:	e8 57 fe ff ff       	call   400850 <getpwuid@plt>
  4009f9:	48 8b 30             	mov    (%rax),%rsi
  4009fc:	48 83 c9 ff          	or     $0xffffffffffffffff,%rcx
  400a00:	31 c0                	xor    %eax,%eax
  400a02:	48 89 f7             	mov    %rsi,%rdi
  400a05:	f2 ae                	repnz scas %es:(%rdi),%al
  400a07:	48 f7 d1             	not    %rcx
  400a0a:	48 83 e9 01          	sub    $0x1,%rcx
  400a0e:	0f 84 61 01 00 00    	je     400b75 <open@plt+0x245>
  400a14:	48 8d 04 0e          	lea    (%rsi,%rcx,1),%rax
  400a18:	31 d2                	xor    %edx,%edx
  400a1a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
  400a20:	0f b6 0e             	movzbl (%rsi),%ecx
  400a23:	48 83 c6 01          	add    $0x1,%rsi
  400a27:	01 ca                	add    %ecx,%edx
  400a29:	89 d1                	mov    %edx,%ecx
  400a2b:	c1 e1 0a             	shl    $0xa,%ecx
  400a2e:	01 d1                	add    %edx,%ecx
  400a30:	89 ca                	mov    %ecx,%edx
  400a32:	c1 ea 06             	shr    $0x6,%edx
  400a35:	31 ca                	xor    %ecx,%edx
  400a37:	48 39 c6             	cmp    %rax,%rsi
  400a3a:	75 e4                	jne    400a20 <open@plt+0xf0>
  400a3c:	8d 04 d2             	lea    (%rdx,%rdx,8),%eax
  400a3f:	bf 28 0e 40 00       	mov    $0x400e28,%edi
  400a44:	45 31 e4             	xor    %r12d,%r12d
  400a47:	89 c6                	mov    %eax,%esi
  400a49:	c1 ee 0b             	shr    $0xb,%esi
  400a4c:	31 c6                	xor    %eax,%esi
  400a4e:	31 c0                	xor    %eax,%eax
  400a50:	69 f6 01 80 00 00    	imul   $0x8001,%esi,%esi
  400a56:	e8 25 fe ff ff       	call   400880 <printf@plt>
  400a5b:	e9 e1 00 00 00       	jmp    400b41 <open@plt+0x211>
  400a60:	e8 ab fd ff ff       	call   400810 <__errno_location@plt>
  400a65:	83 38 02             	cmpl   $0x2,(%rax)
  400a68:	74 0a                	je     400a74 <open@plt+0x144>
  400a6a:	bf 01 00 00 00       	mov    $0x1,%edi
  400a6f:	e8 2c 02 00 00       	call   400ca0 <open@plt+0x370>
  400a74:	bf 95 0e 40 00       	mov    $0x400e95,%edi
  400a79:	e8 b2 fd ff ff       	call   400830 <puts@plt>
  400a7e:	be 2a 00 00 00       	mov    $0x2a,%esi
  400a83:	89 df                	mov    %ebx,%edi
  400a85:	e8 16 fe ff ff       	call   4008a0 <ftruncate@plt>
  400a8a:	85 c0                	test   %eax,%eax
  400a8c:	0f 85 c3 00 00 00    	jne    400b55 <open@plt+0x225>
  400a92:	45 31 c9             	xor    %r9d,%r9d
  400a95:	31 ff                	xor    %edi,%edi
  400a97:	41 89 d8             	mov    %ebx,%r8d
  400a9a:	b9 01 00 00 00       	mov    $0x1,%ecx
  400a9f:	ba 02 00 00 00       	mov    $0x2,%edx
  400aa4:	be 2a 00 00 00       	mov    $0x2a,%esi
  400aa9:	e8 c2 fd ff ff       	call   400870 <mmap@plt>
  400aae:	48 83 f8 ff          	cmp    $0xffffffffffffffff,%rax
  400ab2:	48 89 c5             	mov    %rax,%rbp
  400ab5:	0f 84 a9 00 00 00    	je     400b64 <open@plt+0x234>
  400abb:	31 c0                	xor    %eax,%eax
  400abd:	e8 fe 01 00 00       	call   400cc0 <open@plt+0x390>
  400ac2:	ba ef be ad de       	mov    $0xdeadbeef,%edx
  400ac7:	be 2a 00 00 00       	mov    $0x2a,%esi
  400acc:	48 89 ef             	mov    %rbp,%rdi
  400acf:	48 31 d0             	xor    %rdx,%rax
  400ad2:	ba ad 0e 40 00       	mov    $0x400ead,%edx
  400ad7:	48 89 c1             	mov    %rax,%rcx
  400ada:	31 c0                	xor    %eax,%eax
  400adc:	e8 af fd ff ff       	call   400890 <snprintf@plt>
  400ae1:	48 98                	cltq   
  400ae3:	48 83 f8 29          	cmp    $0x29,%rax
  400ae7:	0f 87 8f 00 00 00    	ja     400b7c <open@plt+0x24c>
  400aed:	be 2a 00 00 00       	mov    $0x2a,%esi
  400af2:	48 89 ef             	mov    %rbp,%rdi
  400af5:	e8 26 fe ff ff       	call   400920 <munmap@plt>
  400afa:	89 df                	mov    %ebx,%edi
  400afc:	e8 cf fd ff ff       	call   4008d0 <close@plt>
  400b01:	b8 01 00 00 00       	mov    $0x1,%eax
  400b06:	48 83 c4 10          	add    $0x10,%rsp
  400b0a:	5b                   	pop    %rbx
  400b0b:	5d                   	pop    %rbp
  400b0c:	41 5c                	pop    %r12
  400b0e:	c3                   	ret    
  400b0f:	e8 fc fc ff ff       	call   400810 <__errno_location@plt>
  400b14:	83 38 02             	cmpl   $0x2,(%rax)
  400b17:	74 07                	je     400b20 <open@plt+0x1f0>
  400b19:	31 ff                	xor    %edi,%edi
  400b1b:	e8 80 01 00 00       	call   400ca0 <open@plt+0x370>
  400b20:	bf 00 0e 40 00       	mov    $0x400e00,%edi
  400b25:	e8 06 fd ff ff       	call   400830 <puts@plt>
  400b2a:	b8 01 00 00 00       	mov    $0x1,%eax
  400b2f:	eb d5                	jmp    400b06 <open@plt+0x1d6>
  400b31:	bf fc 0e 40 00       	mov    $0x400efc,%edi
  400b36:	41 bc 01 00 00 00    	mov    $0x1,%r12d
  400b3c:	e8 ef fc ff ff       	call   400830 <puts@plt>
  400b41:	48 89 df             	mov    %rbx,%rdi
  400b44:	e8 f7 fc ff ff       	call   400840 <fclose@plt>
  400b49:	89 ef                	mov    %ebp,%edi
  400b4b:	e8 80 fd ff ff       	call   4008d0 <close@plt>
  400b50:	44 89 e0             	mov    %r12d,%eax
  400b53:	eb b1                	jmp    400b06 <open@plt+0x1d6>
  400b55:	bf 03 00 00 00       	mov    $0x3,%edi
  400b5a:	e8 41 01 00 00       	call   400ca0 <open@plt+0x370>
  400b5f:	e9 2e ff ff ff       	jmp    400a92 <open@plt+0x162>
  400b64:	bf 02 00 00 00       	mov    $0x2,%edi
  400b69:	e8 32 01 00 00       	call   400ca0 <open@plt+0x370>
  400b6e:	66 90                	xchg   %ax,%ax
  400b70:	e9 46 ff ff ff       	jmp    400abb <open@plt+0x18b>
  400b75:	31 d2                	xor    %edx,%edx
  400b77:	e9 c0 fe ff ff       	jmp    400a3c <open@plt+0x10c>
  400b7c:	b9 10 0f 40 00       	mov    $0x400f10,%ecx
  400b81:	ba 63 00 00 00       	mov    $0x63,%edx
  400b86:	be bd 0e 40 00       	mov    $0x400ebd,%esi
  400b8b:	bf c6 0e 40 00       	mov    $0x400ec6,%edi
  400b90:	e8 2b fd ff ff       	call   4008c0 <__assert_fail@plt>
  400b95:	b9 10 0f 40 00       	mov    $0x400f10,%ecx
  400b9a:	ba 6c 00 00 00       	mov    $0x6c,%edx
  400b9f:	be bd 0e 40 00       	mov    $0x400ebd,%esi
  400ba4:	bf e2 0e 40 00       	mov    $0x400ee2,%edi
  400ba9:	e8 12 fd ff ff       	call   4008c0 <__assert_fail@plt>
  400bae:	31 ed                	xor    %ebp,%ebp
  400bb0:	49 89 d1             	mov    %rdx,%r9
  400bb3:	5e                   	pop    %rsi
  400bb4:	48 89 e2             	mov    %rsp,%rdx
  400bb7:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  400bbb:	50                   	push   %rax
  400bbc:	54                   	push   %rsp
  400bbd:	49 c7 c0 60 0d 40 00 	mov    $0x400d60,%r8
  400bc4:	48 c7 c1 f0 0c 40 00 	mov    $0x400cf0,%rcx
  400bcb:	48 c7 c7 40 09 40 00 	mov    $0x400940,%rdi
  400bd2:	e8 09 fd ff ff       	call   4008e0 <__libc_start_main@plt>
  400bd7:	f4                   	hlt    
  400bd8:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  400bdf:	00 
  400be0:	b8 bf 20 60 00       	mov    $0x6020bf,%eax
  400be5:	55                   	push   %rbp
  400be6:	48 2d b8 20 60 00    	sub    $0x6020b8,%rax
  400bec:	48 83 f8 0e          	cmp    $0xe,%rax
  400bf0:	48 89 e5             	mov    %rsp,%rbp
  400bf3:	77 02                	ja     400bf7 <open@plt+0x2c7>
  400bf5:	5d                   	pop    %rbp
  400bf6:	c3                   	ret    
  400bf7:	b8 00 00 00 00       	mov    $0x0,%eax
  400bfc:	48 85 c0             	test   %rax,%rax
  400bff:	74 f4                	je     400bf5 <open@plt+0x2c5>
  400c01:	5d                   	pop    %rbp
  400c02:	bf b8 20 60 00       	mov    $0x6020b8,%edi
  400c07:	ff e0                	jmp    *%rax
  400c09:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  400c10:	b8 b8 20 60 00       	mov    $0x6020b8,%eax
  400c15:	55                   	push   %rbp
  400c16:	48 2d b8 20 60 00    	sub    $0x6020b8,%rax
  400c1c:	48 c1 f8 03          	sar    $0x3,%rax
  400c20:	48 89 e5             	mov    %rsp,%rbp
  400c23:	48 89 c2             	mov    %rax,%rdx
  400c26:	48 c1 ea 3f          	shr    $0x3f,%rdx
  400c2a:	48 01 d0             	add    %rdx,%rax
  400c2d:	48 d1 f8             	sar    %rax
  400c30:	75 02                	jne    400c34 <open@plt+0x304>
  400c32:	5d                   	pop    %rbp
  400c33:	c3                   	ret    
  400c34:	ba 00 00 00 00       	mov    $0x0,%edx
  400c39:	48 85 d2             	test   %rdx,%rdx
  400c3c:	74 f4                	je     400c32 <open@plt+0x302>
  400c3e:	5d                   	pop    %rbp
  400c3f:	48 89 c6             	mov    %rax,%rsi
  400c42:	bf b8 20 60 00       	mov    $0x6020b8,%edi
  400c47:	ff e2                	jmp    *%rdx
  400c49:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
  400c50:	80 3d 69 14 20 00 00 	cmpb   $0x0,0x201469(%rip)        # 6020c0 <stderr@GLIBC_2.2.5+0x8>
  400c57:	75 11                	jne    400c6a <open@plt+0x33a>
  400c59:	55                   	push   %rbp
  400c5a:	48 89 e5             	mov    %rsp,%rbp
  400c5d:	e8 7e ff ff ff       	call   400be0 <open@plt+0x2b0>
  400c62:	5d                   	pop    %rbp
  400c63:	c6 05 56 14 20 00 01 	movb   $0x1,0x201456(%rip)        # 6020c0 <stderr@GLIBC_2.2.5+0x8>
  400c6a:	f3 c3                	repz ret 
  400c6c:	0f 1f 40 00          	nopl   0x0(%rax)
  400c70:	48 83 3d a8 11 20 00 	cmpq   $0x0,0x2011a8(%rip)        # 601e20 <open@plt+0x2014f0>
  400c77:	00 
  400c78:	74 1e                	je     400c98 <open@plt+0x368>
  400c7a:	b8 00 00 00 00       	mov    $0x0,%eax
  400c7f:	48 85 c0             	test   %rax,%rax
  400c82:	74 14                	je     400c98 <open@plt+0x368>
  400c84:	55                   	push   %rbp
  400c85:	bf 20 1e 60 00       	mov    $0x601e20,%edi
  400c8a:	48 89 e5             	mov    %rsp,%rbp
  400c8d:	ff d0                	call   *%rax
  400c8f:	5d                   	pop    %rbp
  400c90:	e9 7b ff ff ff       	jmp    400c10 <open@plt+0x2e0>
  400c95:	0f 1f 00             	nopl   (%rax)
  400c98:	e9 73 ff ff ff       	jmp    400c10 <open@plt+0x2e0>
  400c9d:	0f 1f 00             	nopl   (%rax)
  400ca0:	53                   	push   %rbx
  400ca1:	89 fb                	mov    %edi,%ebx
  400ca3:	e8 68 fb ff ff       	call   400810 <__errno_location@plt>
  400ca8:	89 da                	mov    %ebx,%edx
  400caa:	8b 08                	mov    (%rax),%ecx
  400cac:	48 8b 3d 05 14 20 00 	mov    0x201405(%rip),%rdi        # 6020b8 <stderr@GLIBC_2.2.5>
  400cb3:	5b                   	pop    %rbx
  400cb4:	be 80 0d 40 00       	mov    $0x400d80,%esi
  400cb9:	31 c0                	xor    %eax,%eax
  400cbb:	e9 30 fc ff ff       	jmp    4008f0 <fprintf@plt>
  400cc0:	48 83 ec 18          	sub    $0x18,%rsp
  400cc4:	31 f6                	xor    %esi,%esi
  400cc6:	48 89 e7             	mov    %rsp,%rdi
  400cc9:	e8 e2 fb ff ff       	call   4008b0 <gettimeofday@plt>
  400cce:	85 c0                	test   %eax,%eax
  400cd0:	74 0a                	je     400cdc <open@plt+0x3ac>
  400cd2:	bf 04 00 00 00       	mov    $0x4,%edi
  400cd7:	e8 c4 ff ff ff       	call   400ca0 <open@plt+0x370>
  400cdc:	48 8b 04 24          	mov    (%rsp),%rax
  400ce0:	48 83 c4 18          	add    $0x18,%rsp
  400ce4:	c3                   	ret    
  400ce5:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  400cec:	00 00 00 
  400cef:	90                   	nop
  400cf0:	41 57                	push   %r15
  400cf2:	41 89 ff             	mov    %edi,%r15d
  400cf5:	41 56                	push   %r14
  400cf7:	49 89 f6             	mov    %rsi,%r14
  400cfa:	41 55                	push   %r13
  400cfc:	49 89 d5             	mov    %rdx,%r13
  400cff:	41 54                	push   %r12
  400d01:	4c 8d 25 08 11 20 00 	lea    0x201108(%rip),%r12        # 601e10 <open@plt+0x2014e0>
  400d08:	55                   	push   %rbp
  400d09:	48 8d 2d 08 11 20 00 	lea    0x201108(%rip),%rbp        # 601e18 <open@plt+0x2014e8>
  400d10:	53                   	push   %rbx
  400d11:	4c 29 e5             	sub    %r12,%rbp
  400d14:	31 db                	xor    %ebx,%ebx
  400d16:	48 c1 fd 03          	sar    $0x3,%rbp
  400d1a:	48 83 ec 08          	sub    $0x8,%rsp
  400d1e:	e8 bd fa ff ff       	call   4007e0 <__errno_location@plt-0x30>
  400d23:	48 85 ed             	test   %rbp,%rbp
  400d26:	74 1e                	je     400d46 <open@plt+0x416>
  400d28:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  400d2f:	00 
  400d30:	4c 89 ea             	mov    %r13,%rdx
  400d33:	4c 89 f6             	mov    %r14,%rsi
  400d36:	44 89 ff             	mov    %r15d,%edi
  400d39:	41 ff 14 dc          	call   *(%r12,%rbx,8)
  400d3d:	48 83 c3 01          	add    $0x1,%rbx
  400d41:	48 39 eb             	cmp    %rbp,%rbx
  400d44:	75 ea                	jne    400d30 <open@plt+0x400>
  400d46:	48 83 c4 08          	add    $0x8,%rsp
  400d4a:	5b                   	pop    %rbx
  400d4b:	5d                   	pop    %rbp
  400d4c:	41 5c                	pop    %r12
  400d4e:	41 5d                	pop    %r13
  400d50:	41 5e                	pop    %r14
  400d52:	41 5f                	pop    %r15
  400d54:	c3                   	ret    
  400d55:	90                   	nop
  400d56:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  400d5d:	00 00 00 
  400d60:	f3 c3                	repz ret 
