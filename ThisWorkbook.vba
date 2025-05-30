Private Sub Workbook_Open()
    Dim extFilePath As String
    Dim wb As Workbook
    Dim dejaDeschis As Boolean

    extFilePath = ThisWorkbook.Path & "\BDFisaPV.xlsx"

    ' Verificăm dacă fi?ierul este deja deschis
    dejaDeschis = False
    For Each wb In Workbooks
        If StrComp(wb.FullName, extFilePath, vbTextCompare) = 0 Then
            Set wbBDFisaPV = wb
            dejaDeschis = True
            Exit For
        End If
    Next wb

    ' Încercăm să deschidem fi?ierul, tratăm eventualele erori
    If Not dejaDeschis Then
        On Error GoTo ErrOpen
        Set wbBDFisaPV = Workbooks.Open(extFilePath, ReadOnly:=True)
        On Error Resume Next
        wbBDFisaPV.Windows(1).Visible = True
        On Error GoTo 0
    End If
    Exit Sub

ErrOpen:
    MsgBox "Nu s-a putut deschide '" & extFilePath & "'!" & vbCrLf & "Verifică dacă fi?ierul există ?i ai acces la el.", vbCritical
End Sub

Private Sub Workbook_BeforeClose(Cancel As Boolean)
    On Error Resume Next
    If Not wbBDFisaPV Is Nothing Then
        wbBDFisaPV.Close SaveChanges:=False
        Set wbBDFisaPV = Nothing
    End If
    On Error GoTo 0
End Sub
