/****************************************************************************
 * libwiigui
 *
 * gave92 2012
 *
 * gui_favorite.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "main.h"
#include "gui.h"

/**
 * Constructor for the GuiFavorite class.
 */
GuiFavorite::GuiFavorite()
{
    editing = 0;
    btnSound = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM);
    trigA = new GuiTrigger();
    trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

    BlockData = new GuiImageData(button_large_png);
    BlockDataOver = new GuiImageData(button_large_over_png);
    RemoveData = new GuiImageData(remove_png);
    RemoveDataOver = new GuiImageData(remove_over_png);

    BlockImg = new GuiImage(BlockData);
    BlockImgOver = new GuiImage(BlockDataOver);
    Block = new GuiButton(BlockData->GetWidth(), BlockData->GetHeight());
    Block->SetImage(BlockImg);
    Block->SetImageOver(BlockImgOver);
    Block->SetSoundOver(btnSound);
    Block->SetTrigger(trigA);
    Block->SetEffectGrow();

    RemoveImg = new GuiImage(RemoveData);
    RemoveImgOver = new GuiImage(RemoveDataOver);
    Remove = new GuiButton(RemoveData->GetWidth(), RemoveData->GetHeight());
    Remove->SetImage(RemoveImg);
    Remove->SetImageOver(RemoveImgOver);
    Remove->SetSoundOver(btnSound);
    Remove->SetTrigger(trigA);
    Remove->SetEffectGrow();
    Remove->SetPosition(BlockData->GetWidth()-30,-20);
    Remove->SetVisible(false);

    this->Append(Block);
    this->Append(Remove);
}

/**
 * Destructor for the GuiSwitch class.
 */
GuiFavorite::~GuiFavorite()
{
    delete(btnSound);
    delete(trigA);

    delete(BlockData);
    delete(BlockDataOver);
    delete(RemoveData);
    delete(RemoveDataOver);

    delete(RemoveImg);
    delete(RemoveImgOver);
    delete(Remove);

    delete(BlockImg);
    delete(BlockImgOver);
    delete(Block);
}

void GuiFavorite::SetEditing(bool e)
{
    editing = e;
    this->Remove->SetVisible(editing);

    if(editing)
    {
        this->Remove->SetEffect(EFFECT_FADE, 30);
        this->BlockImg->SetEffect(EFFECT_RUMBLE,2,5);
    }

    else
    {
        this->Remove->SetEffect(EFFECT_FADE, -30);
        this->BlockImg->StopEffect(EFFECT_RUMBLE);
        this->BlockImg->SetAngle(0);
    }
}

int GuiFavorite::GetDataWidth()
{
    return BlockData->GetWidth();
}

int GuiFavorite::GetDataHeight()
{
    return BlockData->GetHeight();
}

void GuiFavorite::Update(GuiTrigger * t)
{
	if(_elements.size() == 0 || (state == STATE_DISABLED && parentElement))
		return;

	for (u8 i = 0; i < _elements.size(); i++)
	{
		try	{ _elements.at(i)->Update(t); }
		catch (const std::exception& e) { }
	}

	this->ToggleFocus(t);

	if(focus) // only send actions to this window if it's in focus
	{
		// pad/joystick navigation
		if(t->Right())
			this->MoveSelectionHor(1);
		else if(t->Left())
			this->MoveSelectionHor(-1);
		else if(t->Down())
			this->MoveSelectionVert(1);
		else if(t->Up())
			this->MoveSelectionVert(-1);
	}
}
