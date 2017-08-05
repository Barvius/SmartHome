import { MhPage } from './app.po';

describe('mh App', () => {
  let page: MhPage;

  beforeEach(() => {
    page = new MhPage();
  });

  it('should display welcome message', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('Welcome to app!');
  });
});
