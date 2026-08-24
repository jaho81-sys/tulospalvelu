object FormApiYhteydet: TFormApiYhteydet
  Left = 0
  Top = 0
  Width = 600
  Height = 550
  Caption = 'REST API - Verkko yhteydet'
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
    Width = 600
    Height = 480
    ActivePage = TabYhteys
    Align = alClient
    TabOrder = 0
    object TabYhteys: TTabSheet
      Caption = 'Yhteysasetukset'
      object LabelUrl: TLabel
        Left = 20
        Top = 30
        Width = 140
        Height = 13
        Caption = 'API-palvelimen osoite (URL):'
      end
      object LabelPortti: TLabel
        Left = 20
        Top = 80
        Width = 86
        Height = 13
        Caption = 'Portti (port):'
      end
      object LabelApiKey: TLabel
        Left = 20
        Top = 130
        Width = 132
        Height = 13
        Caption = 'API-avain (authentication):'
      end
      object LabelYhteysTila: TLabel
        Left = 20
        Top = 200
        Width = 100
        Height = 20
        Caption = 'EI AKTIIVINEN'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -16
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object EditUrl: TEdit
        Left = 20
        Top = 50
        Width = 300
        Height = 21
        Text = 'http://localhost'
        TabOrder = 0
        OnChange = EditUrlChange
      end
      object EditPortti: TEdit
        Left = 20
        Top = 100
        Width = 100
        Height = 21
        Text = '8080'
        TabOrder = 1
      end
      object EditApiKey: TEdit
        Left = 20
        Top = 150
        Width = 300
        Height = 21
        PasswordChar = '*'
        TabOrder = 2
      end
      object BtnYhteysTesti: TButton
        Left = 20
        Top = 240
        Width = 150
        Height = 35
        Caption = 'Testaa yhteyttä'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        OnClick = BtnYhteysTesti
      end
    end
    object TabAsetukset: TTabSheet
      Caption = 'Automaattiset asetukset'
      ImageIndex = 1
      object CBLahetaValiajat: TCheckBox
        Left = 20
        Top = 30
        Width = 400
        Height = 17
        Caption = 'Lähetä väliajat automaattisesti nettiin'
        TabOrder = 0
      end
      object CBVastaanottaValiajat: TCheckBox
        Left = 20
        Top = 60
        Width = 400
        Height = 17
        Caption = 'Vastaanota väliajat mobiilisovelluksesta / netistä'
        TabOrder = 1
      end
      object CBLahetaTulokset: TCheckBox
        Left = 20
        Top = 90
        Width = 400
        Height = 17
        Caption = 'Lähetä tulokset nettiin (HTML/JSON)'
        TabOrder = 2
      end
      object CBVastaanottaEiLahteneet: TCheckBox
        Left = 20
        Top = 120
        Width = 400
        Height = 17
        Caption = 'Vastaanota "ei lähteneet" merkinnät netistä'
        TabOrder = 3
      end
      object LabelLahetysvali: TLabel
        Left = 20
        Top = 160
        Width = 130
        Height = 13
        Caption = 'Lähettämisen väli:'
      end
      object EditLahetysvali: TEdit
        Left = 20
        Top = 180
        Width = 80
        Height = 21
        Text = '5'
        TabOrder = 4
      end
      object LabelSekunti: TLabel
        Left = 110
        Top = 185
        Width = 60
        Height = 13
        Caption = 'sekuntia'
      end
    end
    object TabTila: TTabSheet
      Caption = 'Yhteyden tila & lokit'
      ImageIndex = 2
      object MemoTila: TMemo
        Left = 10
        Top = 10
        Width = 560
        Height = 380
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -10
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
      end
      object BtnPaivitaTila: TButton
        Left = 10
        Top = 400
        Width = 100
        Height = 25
        Caption = 'Päivitä tila'
        TabOrder = 1
        OnClick = BtnPaivitaTilaClick
      end
      object BtnTyhjennaMemo: TButton
        Left = 120
        Top = 400
        Width = 100
        Height = 25
        Caption = 'Tyhjennä lokit'
        TabOrder = 2
        OnClick = BtnTyhjennaMemoClick
      end
      object BtnVieInternet: TButton
        Left = 230
        Top = 400
        Width = 150
        Height = 25
        Caption = 'Ohjeet (www)'
        TabOrder = 3
        OnClick = BtnVieInternetClick
      end
    end
  end
  object BtnOK: TButton
    Left = 360
    Top = 500
    Width = 100
    Height = 30
    Caption = 'OK'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    OnClick = BtnOKClick
  end
  object BtnPeruuta: TButton
    Left = 470
    Top = 500
    Width = 100
    Height = 30
    Caption = 'Peruuta'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = BtnPeruutaClick
  end
  object BtnOhje: TButton
    Left = 10
    Top = 500
    Width = 100
    Height = 30
    Caption = '? Ohje'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = BtnOhjeClick
  end
end
