object FormApiYhteydet: TFormApiYhteydet
  Left = 0
  Top = 0
  Caption = 'JAHOnline API - synkka'
  ClientHeight = 560
  ClientWidth = 620
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 620
    Height = 490
    ActivePage = TabYhteys
    Align = alClient
    TabOrder = 0
    object TabYhteys: TTabSheet
      Caption = 'Yhteys'
      object LabelUrl: TLabel
        Left = 16
        Top = 16
        Width = 200
        Height = 13
        Caption = 'Bridge-URL (JAHOnline):'
      end
      object LabelPortti: TLabel
        Left = 16
        Top = 72
        Width = 180
        Height = 13
        Caption = 'Portti (0 = URL:n oletus):'
      end
      object LabelApiKey: TLabel
        Left = 16
        Top = 128
        Width = 220
        Height = 13
        Caption = 'API-avain (kilpailun api_token):'
      end
      object LabelKilpailuId: TLabel
        Left = 16
        Top = 184
        Width = 160
        Height = 13
        Caption = 'kilpailu_id (JAHOnline):'
      end
      object LabelYhteysTila: TLabel
        Left = 16
        Top = 280
        Width = 120
        Height = 20
        Caption = 'EI AKTIIVINEN'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -16
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object LabelIniPolku: TLabel
        Left = 16
        Top = 236
        Width = 560
        Height = 36
        AutoSize = False
        Caption = 'Asetustiedosto: kilpailun kansio\jahonline_api.ini'
        WordWrap = True
      end
      object EditUrl: TEdit
        Left = 16
        Top = 36
        Width = 560
        Height = 21
        TabOrder = 0
        Text = 'https://jahonline.com/public/api/kilpailijat_bridge.php'
        OnChange = EditUrlChange
      end
      object EditPortti: TEdit
        Left = 16
        Top = 92
        Width = 80
        Height = 21
        TabOrder = 1
        Text = '0'
      end
      object EditApiKey: TEdit
        Left = 16
        Top = 148
        Width = 400
        Height = 21
        PasswordChar = '*'
        TabOrder = 2
      end
      object EditKilpailuId: TEdit
        Left = 16
        Top = 204
        Width = 100
        Height = 21
        TabOrder = 3
        Text = '0'
      end
      object BtnYhteysTesti: TButton
        Left = 16
        Top = 320
        Width = 140
        Height = 32
        Caption = 'Testaa (ping)'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 4
        OnClick = BtnYhteysTestiClick
      end
      object BtnLahetaNyt: TButton
        Left = 170
        Top = 320
        Width = 160
        Height = 32
        Caption = 'L'#228'het'#228' kilpailijat nyt'
        TabOrder = 5
        OnClick = BtnLahetaNytClick
      end
      object BtnHaeNyt: TButton
        Left = 348
        Top = 320
        Width = 160
        Height = 32
        Caption = 'Hae kilpailijat nyt'
        TabOrder = 6
        OnClick = BtnHaeNytClick
      end
    end
    object TabAsetukset: TTabSheet
      Caption = 'Automaatio'
      ImageIndex = 1
      object CBLahetaKilpailijat: TCheckBox
        Left = 16
        Top = 24
        Width = 520
        Height = 17
        Caption = 'L'#228'het'#228' kaikki kilpailijat + ajat automaattisesti (synkkaa)'
        Checked = True
        State = cbChecked
        TabOrder = 0
      end
      object CBVastaanottaKilpailijat: TCheckBox
        Left = 16
        Top = 52
        Width = 520
        Height = 17
        Caption = 'Vastaanota kilpailijat JAHOnlinesta (kilpailijat)'
        Checked = True
        State = cbChecked
        TabOrder = 1
      end
      object CBLahetaValiajat: TCheckBox
        Left = 16
        Top = 80
        Width = 520
        Height = 17
        Caption = 'L'#228'het'#228' live-v'#228'liajat / online-rastit (tapahtuma)'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
      object CBVastaanottaValiajat: TCheckBox
        Left = 16
        Top = 108
        Width = 520
        Height = 17
        Caption = 'Vastaanota v'#228'liajat / online-rastit JAHOnlinesta'
        Checked = True
        State = cbChecked
        TabOrder = 3
      end
      object CBLahetaTulokset: TCheckBox
        Left = 16
        Top = 136
        Width = 520
        Height = 17
        Caption = 'Sis'#228'lyt'#228' tulokset l'#228'hetykseen'
        Checked = True
        State = cbChecked
        TabOrder = 4
      end
      object CBVastaanottaEiLahteneet: TCheckBox
        Left = 16
        Top = 164
        Width = 520
        Height = 17
        Caption = 'P'#228'ivit'#228' DNS/DNF-statukset hausta'
        Checked = True
        State = cbChecked
        TabOrder = 5
      end
      object LabelLahetysvali: TLabel
        Left = 16
        Top = 208
        Width = 120
        Height = 13
        Caption = 'Synkronointiv'#228'li:'
      end
      object EditLahetysvali: TEdit
        Left = 16
        Top = 228
        Width = 60
        Height = 21
        TabOrder = 6
        Text = '10'
      end
      object LabelSekunti: TLabel
        Left = 84
        Top = 232
        Width = 50
        Height = 13
        Caption = 'sekuntia'
      end
    end
    object TabTila: TTabSheet
      Caption = 'Loki'
      ImageIndex = 2
      object MemoTila: TMemo
        Left = 8
        Top = 8
        Width = 580
        Height = 380
        Font.Name = 'Consolas'
        Font.Height = -11
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
      object BtnPaivitaTila: TButton
        Left = 8
        Top = 400
        Width = 100
        Height = 25
        Caption = 'P'#228'ivit'#228
        TabOrder = 1
        OnClick = BtnPaivitaTilaClick
      end
      object BtnTyhjennaMemo: TButton
        Left = 120
        Top = 400
        Width = 110
        Height = 25
        Caption = 'Tyhjenn'#228' loki'
        TabOrder = 2
        OnClick = BtnTyhjennaMemoClick
      end
      object BtnVieInternet: TButton
        Left = 244
        Top = 400
        Width = 160
        Height = 25
        Caption = 'Avaa API-ohje'
        TabOrder = 3
        OnClick = BtnVieInternetClick
      end
    end
  end
  object BtnOK: TButton
    Left = 320
    Top = 510
    Width = 90
    Height = 30
    Caption = 'OK'
    TabOrder = 1
    OnClick = BtnOKClick
  end
  object BtnPeruuta: TButton
    Left = 420
    Top = 510
    Width = 90
    Height = 30
    Caption = 'Peruuta'
    TabOrder = 2
    OnClick = BtnPeruutaClick
  end
  object BtnOhje: TButton
    Left = 16
    Top = 510
    Width = 90
    Height = 30
    Caption = 'Ohje'
    TabOrder = 3
    OnClick = BtnOhjeClick
  end
end
