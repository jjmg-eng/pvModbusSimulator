//###############################################################
//# mask1_slots.h for ProcessViewServer created: ter mai 7 07:25:59 2024
//# please fill out these slots
//# here you find all possible events
//# Yours: Lehrig Software Engineering
//###############################################################

// todo: uncomment me if you want to use this data aquisiton
// also uncomment this classes in main.cpp and pvapp.h
// also remember to uncomment rllib in the project file
extern rlModbusClient     modbus;  //Change if applicable
//extern rlSiemensTCPClient siemensTCP;
//extern rlPPIClient        ppi;


typedef struct // (todo: define your data structure here)
{
int init;
int AI;
int c[8][2];
}
DATA;

static int slotInit(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  memset(d,0,sizeof(DATA));
  pvResize(p,0,1024,768);
  int i;
// Configuração da tabela DI/DO
  char buf[32];
  int id = Table1;
  int x,y,z;
  pvSetRowHeight(p,id,-1,1);
  pvSetColumnWidth(p,id,-1,0);
  pvSetEditable(p,id,0);
  for(z = y = 0; y<2; y++)
  for(    x = 0; x<8; x++){
      pvSetColumnWidth(p,id,x,127);
      i = !!modbus.readBit(modbusdaemon_CYCLE1_BASE,z++);
      d->c[x][y] = i;
      sprintf(buf,"color(%d,%d,%d)D%c%d", 228+27*i, 228-127*i, 228-127*i, y ? 'O' : 'I', x+1);
      pvSetTableButton(p,id,x,y,buf);
  }
// Configuração da tabela de sinais
  id = Table2;
  pvSetEditable(p,id,0);
  x = 0; y = -1;
  pvTablePrintf(p,id,x++,y,"Amplitude");
  pvTablePrintf(p,id,x++,y,"Período");
  pvTablePrintf(p,id,x++,y,"Fase");
  pvTablePrintf(p,id,x++,y,"Offset");
  pvTablePrintf(p,id,x++,y,"Valor");
  x = -1;
  for(y = 0; y<8; y++) pvTablePrintf(p,id,x,y,"AI%d",y+1);
  for(x = -1; x<5; x++) pvSetColumnWidth(p,id,x,169);
// Configuração do seletor
  id = AIselect;
 // pvSetEnabled(p,id,1); 
  for(i=0; i<8; i++) {
      sprintf(buf,"AI%d", i + 1);
      pvInsertItem(p,id,-1,NULL,buf);
  }
// Configuração dos knobs
  id = AMP;
  qwtKnobSetScale(p,id,0,100,25,0);
  qwtKnobSetValue(p,id,AI[d->AI].amp/100.0);
  qwtKnobSetKnobWidth(p,id,60);

  id = T;
  qwtKnobSetScale(p,id,5,100,25,0);
  qwtKnobSetValue(p,id,AI[d->AI].T/100.0);
  qwtKnobSetKnobWidth(p,id,60);

  id = FASE;
  qwtKnobSetScale(p,id,0,360,60,0);
  qwtKnobSetValue(p,id,AI[d->AI].fase);
  qwtKnobSetKnobWidth(p,id,60);

  id = OFFSET;
  qwtKnobSetScale(p,id,0,100,25,0);
  qwtKnobSetValue(p,id,AI[d->AI].offset);
  qwtKnobSetKnobWidth(p,id,60);

//****************************************************************************
  return 0;
}

static int slotNullEvent(PARAM *p, DATA *d)
{
  if(p == NULL || d == NULL) return -1;
  int i, AIoffset = modbusdaemon_CYCLE3_BASE;

//****************************************************************************
// Atualização da tabela 1
  int id = Table1;
  int x, y;
  for(y = 0; y<2; y++)
  for(x = 0; x<8; x++){
      i = y ? !!modbus.readBit(modbusdaemon_CYCLE2_BASE,x) : !!modbus.readBit(0,x);
      if(d->c[x][y] != i){
          d->c[x][y] = i;
          char buf[32];
          sprintf(buf,"color(%d,%d,%d)D%c%d", 228+27*i, 228-127*i, 228-127*i, y ? 'O' : 'I', x+1);
          pvSetTableButton(p,id,x,y,buf);
      }
  }
//****************************************************************************
// Atualização da tabela 2
  id = Table2;
  for(y = 0; y<8; y++){
      x = 0;
      i = !!(d->AI == y);
      pvTablePrintf(p,id,x++,y,"color(%d,%d,%d)%g [%]",  255, 255-15*i, 255-100*i, AI[y].amp/100.0);
      pvTablePrintf(p,id,x++,y,"color(%d,%d,%d)%g [s]",  255, 255-15*i, 255-100*i, AI[y].T/100.0);
      pvTablePrintf(p,id,x++,y,"color(%d,%d,%d)%d°",     255, 255-15*i, 255-100*i, AI[y].fase);
      pvTablePrintf(p,id,x++,y,"color(%d,%d,%d)%d [%]",  255, 255-15*i, 255-100*i, AI[y].offset);
      pvTablePrintf(p,id,x++,y,"color(%d,%d,%d)%g [mA]", 255, 255-15*i, 255-100*i, modbus.readShort(AIoffset,y)/1000.0);
  }
//****************************************************************************

  if(d->init < 100) d->init++;
  return 0;

}

static int slotButtonEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotButtonPressedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotButtonReleasedEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotTextEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  switch(id){
  	case AIselect:{
		d->AI = atoi( text + 2 ) - 1;
  		int id_ = AMP;
  		qwtKnobSetValue(p,id_,AI[d->AI].amp/100.0);

  		id_ = T;
  		qwtKnobSetValue(p,id_,AI[d->AI].T/100.0);

  		id_ = FASE;
  		qwtKnobSetValue(p,id_,AI[d->AI].fase);

  		id_ = OFFSET;
  		qwtKnobSetValue(p,id_,AI[d->AI].offset);
	} break;
  }
  return 0;
}

static int slotSliderEvent(PARAM *p, int id, DATA *d, int val)
{
  if(d->init < 10) return 0;
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  switch(id){
	case AMP: {
		AI[d->AI].amp = val * 100;
	} break; 
	case T: {
		AI[d->AI].T = val * 100;
	} break; 
	case FASE: {
		AI[d->AI].fase = val;
	} break; 
	case OFFSET: {
		AI[d->AI].offset = val;
	} break; 
  }
  return 0;
}

static int slotCheckboxEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}

static int slotRadioButtonEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}

static int slotGlInitializeEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotGlPaintEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotGlResizeEvent(PARAM *p, int id, DATA *d, int width, int height)
{
  if(p == NULL || id == 0 || d == NULL || width < 0 || height < 0) return -1;
  return 0;
}

static int slotGlIdleEvent(PARAM *p, int id, DATA *d)
{
  if(p == NULL || id == 0 || d == NULL) return -1;
  return 0;
}

static int slotTabEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000) return -1;
  return 0;
}

static int slotTableTextEvent(PARAM *p, int id, DATA *d, int x, int y, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || text == NULL ) return -1;
  if(d->init < 10) return 0;
  if(id == Table1) {
      int offset = y ? modbusdaemon_CYCLE2_BASE : 0;
      int i =  !modbus.readBit(offset,x);
      char buf[32];
      if(x<4 && y)
          for(int j = 0; j<4; j++) 
              if(j != x){
                  modbus.writeSingleCoil(1,j,0);
                  sprintf(buf,"color(228,228,228)DO%d", j+1);
                  pvSetTableButton(p,id,j,y,buf);
              }
      modbus.writeSingleCoil(2 - y,x,i);
      sprintf(buf,"color(%d,%d,%d)D%c%d", 228+27*i, 228-127*i, 228-127*i, y ? 'O' : 'I', x+1);
      pvSetTableButton(p,id,x,y,buf);
  }
  return 0;
}

static int slotTableClickedEvent(PARAM *p, int id, DATA *d, int x, int y, int button)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000 || button < 0) return -1;
  printf("TableClickedEvent\n");
  return 0;
}

static int slotSelectionEvent(PARAM *p, int id, DATA *d, int val, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || text == NULL) return -1;
  return 0;
}

static int slotClipboardEvent(PARAM *p, int id, DATA *d, int val)
{
  if(p == NULL || id == -1 || d == NULL || val < -1000) return -1;
  return 0;
}

static int slotRightMouseEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  //pvPopupMenu(p,-1,"Menu1,Menu2,,Menu3");
  return 0;
}

static int slotKeyboardEvent(PARAM *p, int id, DATA *d, int val, int modifier)
{
  if(p == NULL || id == 0 || d == NULL || val < -1000 || modifier < -1000) return -1;
  return 0;
}

static int slotMouseMovedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMousePressedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMouseReleasedEvent(PARAM *p, int id, DATA *d, float x, float y)
{
  if(p == NULL || id == 0 || d == NULL || x < -1000 || y < -1000) return -1;
  return 0;
}

static int slotMouseOverEvent(PARAM *p, int id, DATA *d, int enter)
{
  if(p == NULL || id == 0 || d == NULL || enter < -1000) return -1;
  return 0;
}

static int slotUserEvent(PARAM *p, int id, DATA *d, const char *text)
{
  if(p == NULL || id == 0 || d == NULL || text == NULL) return -1;
  return 0;
}
